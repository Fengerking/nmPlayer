/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#include "stdio.h"

#include "ccYUVToRGBMB.h"
#include "ccConstant.h"

#ifdef VOX86
#include <emmintrin.h>
#endif

extern VO_U32 alpha_value;

YUV422INTERLACETORGB16_MB_FUNC_PTR YUV422INTERLACETORGB16_MB;
CC_MB_PTR YUV420PLANARTORGB16_NEWMB;
CC_MB_PTR YUV420PLANARTORGB24_NEWMB;
CC_MB_PTR YUV420PLANARTORGB32_NEWMB;
CC_MB_PTR YUV420PLANARTOARGB32_NEWMB;

/***************
	pix_0 | pix_1
	-------------
	pix_2 | pix_3
****************/

//(c, y, pix_0, pix_2)

#define LOAD_DATA(a3, a7, c, y){\
	a0 = psrc_u[c] - 128;\
	a1 = psrc_v[c] - 128;\
	a3 = psrc_y[y];	\
	a7 = psrc_y[in_stride+y];\
}

#define LOAD_YPIX(a3, x){\
	a3 = psrc_y[(x)];\
}

#define LOAD_UPIX(a3, x){\
	a3 = psrc_u[(x)] - 128;\
}

#define LOAD_VPIX(a3,x){\
	a3 = psrc_v[(x)] - 128;\
}

#define LOAD_4BYTE(a3, x){\
	a3 = *((VO_S32*)(psrc+(x)));\
}

//INT:3210
#define GET_PIX0(a0, a3){\
	a0 = a3 & 0XFF;\
}

#define GET_PIX1(a1, a3){\
	a1 = a3 >> 8;\
	a1 = a1 & 0xFF;\
}

#define GET_PIX2(a7, a3){\
	a7 = a3 >> 16;\
	a7 = a7 & 0xFF;\
}

#define GET_PIX3(a7, a3){\
	a7 = a3 >> 24;\
	a7 = a7 & 0xFF;\
}



// input: a0 = u, a1 = v
// output: a2, a0, a1
#define CAL_UV(){\
	a2 = (a0 * param_tab[2]) >> 20;\
	a0 = (a1 * param_tab[1] + a0 *param_tab[3]) >> 20;\
	a1 = (a1 * param_tab[0]) >> 20;\
}

#define PIX_0(a3){\
	a4  = ccClip31[(a3 + a1 + RBDITHER_P2)>>3];\
	a5  = ccClip63[(a3 - a0 + GDITHER_P0)>>2];\
	a3  = ccClip31[(a3 + a2 + RBDITHER_P0)>>3];\
	a4 = ((((a4<<6)|a5 )<<5))|a3;\
}
#define PIX_1(a3){\
	a5  = ccClip31[(a3 + a2 + RBDITHER_P1)>>3];\
	a4 |= (a5 << 16);\
	a5  = ccClip31[(a3 + a1 + RBDITHER_P3)>>3];\
	a3  = ccClip63[(a3 - a0 + GDITHER_P1)>>2];\
	a3 |= (a5<<6);\
	a4 |= (a3 << 21);\
}

#define PIX_2(a3){\
	a4  = ccClip31[(a3 + a1 + RBDITHER_P0)>>3];\
	a5  = ccClip63[(a3 - a0 + GDITHER_P2)>>2];\
	a3  = ccClip31[(a3 + a2 + RBDITHER_P2)>>3];\
	a4 = ((((a4<<6)|a5 )<<5))|a3;\
}
#define PIX_3(a3){\
	a5  = ccClip31[(a3 + a2 + RBDITHER_P3)>>3];\
	a4 |= (a5 << 16);\
	a5  = ccClip31[(a3 + a1 + RBDITHER_P1)>>3];\
	a3  = ccClip63[(a3 - a0 + GDITHER_P3)>>2];\
	a3 |= (a5<<6);\
	a4 |= (a3 << 21);\
}

#define CAL_UVNEW(){\
	a2_1 = (a0_1 * param_tab[2]) >> 20;\
	a0_1 = (a1_1 * param_tab[1] + a0_1 *param_tab[3]) >> 20;\
	a1_1 = (a1_1 * param_tab[0]) >> 20;\
}

#define PIX_1NEW(a3){\
	a5  = ccClip31[(a3 + a2_1 + RBDITHER_P1)>>3];\
	a4 |= (a5 << 16);\
	a5  = ccClip31[(a3 + a1_1 + RBDITHER_P3)>>3];\
	a3  = ccClip63[(a3 - a0_1 + GDITHER_P1)>>2];\
	a3 |= (a5<<6);\
	a4 |= (a3 << 21);\
}

#define PIX_3NEW(a3){\
	a5  = ccClip31[(a3 + a2_1 + RBDITHER_P3)>>3];\
	a4 |= (a5 << 16);\
	a5  = ccClip31[(a3 + a1_1 + RBDITHER_P1)>>3];\
	a3  = ccClip63[(a3 - a0_1 + GDITHER_P3)>>2];\
	a3 |= (a5<<6);\
	a4 |= (a3 << 21);\
}

#define NEWLOAD_DATA(a3, a7, c, y){\
	a0 = psrc_u[c] - 128;\
	a1 = psrc_v[c] - 128;\
	a3 = (psrc_y[y] - 16)*ConstY;	\
	a7 = (psrc_y[in_stride+y] - 16)*ConstY;\
}

#define NEWLOAD_YPIX(a3, x){\
	a3 = (psrc_y[(x)] - 16)*ConstY;\
}

// input: a0 = u, a1 = v
// output: a2, a0, a1
#define NEWCAL_UV(){\
	a2 = (a0 * ConstU1);\
	a0 = (a1 * ConstV2 + a0 *ConstU2);\
	a1 = (a1 * ConstV1);\
}

#define NEWPIX_0(a3){\
	a4  = ((ccClip63[(a3 + a1)>>22])>>1);\
	a5  = ((ccClip63[(a3 - a0)>>22]));\
	a3  = ((ccClip63[(a3 + a2)>>22])>>1);\
	a4 = ((((a4<<6)|a5 )<<5))|a3;\
}

#define NEWPIX90R_0(a3){\
	a4  = ((ccClip63[(a3 + a1)>>22])>>1);\
	a5  = ((ccClip63[(a3 - a0)>>22]));\
	a3  = ((ccClip63[(a3 + a2)>>22])>>1);\
	a4 = ((((a4<<6)|a5 )<<21))|(a3<<16);\
}

#define NEWPIXRGB32_0(a3){\
	a4  = ((ccClip255[(a3 + a1)>>20]));\
	a5  = ((ccClip255[(a3 - a0)>>20]));\
	a3  = ((ccClip255[(a3 + a2)>>20]));\
	a4 = ((((a4<<8)|a5 )<<8))|a3;\
	a4 |= (alpha_value<< 24) ;\
}

#define NEWPIXARGB32_0(a3){\
	a4  = ((ccClip255[(a3 + a1)>>20]));\
	a5  = ((ccClip255[(a3 - a0)>>20]));\
	a3  = ((ccClip255[(a3 + a2)>>20]));\
	a4 = ((((a3<<8)|a5 )<<8))|(a4);\
	a4 |= (alpha_value<< 24) ;\
}

#define NEWPIX_1(a3){\
	a5  = ((ccClip63[(a3 + a2)>>22])>>1);\
	a4 |= (a5 << 16);\
	a5  = ((ccClip63[(a3 + a1 )>>22])>>1);\
	a3  = (ccClip63[(a3 - a0)>>22]);\
	a3 |= (a5<<6);\
	a4 |= (a3 << 21);\
}

#define NEWPIX90R_1(a3){\
	a5  = ((ccClip63[(a3 + a2)>>22])>>1);\
	a4 |= (a5);\
	a5  = ((ccClip63[(a3 + a1 )>>22])>>1);\
	a3  = (ccClip63[(a3 - a0)>>22]);\
	a3 |= (a5<<6);\
	a4 |= (a3<<5);\
}

#define NEWPIXRGB32_1(a3){\
	a5  = ((ccClip255[(a3 + a2)>>20]));\
	a8  = ((ccClip255[(a3 + a1 )>>20]));\
	a3  = (ccClip255[(a3 - a0)>>20]);\
	a3 |= (a8<< 8);\
	a5 |= (a3<< 8);\
	a5 |= (alpha_value<< 24) ;\
}

#define NEWPIXARGB32_1(a3){\
	a5  = ((ccClip255[(a3 + a2)>>20]));\
	a8  = ((ccClip255[(a3 + a1 )>>20]));\
	a3  = (ccClip255[(a3 - a0)>>20]);\
	a5  = ((a5<<16)|(a3<<8)|a8);\
	a5 |= (alpha_value<< 24) ;\
}

#define NEWPIX_2(a3){\
	a4  = ((ccClip63[(a3 + a1)>>22])>>1);\
	a5  = (ccClip63[(a3 - a0)>>22]);\
	a3  = ((ccClip63[(a3 + a2)>>22])>>1);\
	a4 = ((((a4<<6)|a5 )<<5))|a3;\
}

#define NEWPIX90R_2(a3){\
	a4  = ((ccClip63[(a3 + a1)>>22])>>1);\
	a5  = (ccClip63[(a3 - a0)>>22]);\
	a3  = ((ccClip63[(a3 + a2)>>22])>>1);\
	a4 = ((((a4<<6)|a5 )<<21))|(a3<<16);\
}

#define NEWPIXRGB32_2(a3){\
	a4  = ((ccClip255[(a3 + a1)>>20]));\
	a5  = (ccClip255[(a3 - a0)>>20]);\
	a3  = ((ccClip255[(a3 + a2)>>20]));\
	a4 = ((((a4<<8)|a5 )<<8))|a3;\
	a4 |= (alpha_value<< 24) ;\
}

#define NEWPIXARGB32_2(a3){\
	a4  = ((ccClip255[(a3 + a1)>>20]));\
	a5  = (ccClip255[(a3 - a0)>>20]);\
	a3  = ((ccClip255[(a3 + a2)>>20]));\
	a4 = ((((a3<<8)|a5 )<<8))|(a4);\
	a4 |= (alpha_value<< 24) ;\
}

#define NEWPIX_3(a3){\
	a5  = ((ccClip63[(a3 + a2)>>22])>>1);\
	a4 |= (a5 << 16);\
	a5  = ((ccClip63[(a3 + a1)>>22])>>1);\
	a3  = (ccClip63[(a3 - a0)>>22]);\
	a3 |= (a5<<6);\
	a4 |= (a3 << 21);\
}

#define NEWPIX90R_3(a3){\
	a5  = ((ccClip63[(a3 + a2)>>22])>>1);\
	a4 |= (a5);\
	a5  = ((ccClip63[(a3 + a1)>>22])>>1);\
	a3  = (ccClip63[(a3 - a0)>>22]);\
	a3 |= (a5<<6);\
	a4 |= (a3<<5);\
}

#define NEWPIXRGB32_3(a3){\
	a5  = ((ccClip255[(a3 + a2)>>20]));\
	a8  = ((ccClip255[(a3 + a1)>>20]));\
	a3  = (ccClip255[(a3 - a0)>>20]);\
	a3 |= (a8<<8);\
	a5 |= (a3<<8);\
	a5 |= (alpha_value<< 24) ;\
}

#define NEWPIXARGB32_3(a3){\
	a5  = ((ccClip255[(a3 + a2)>>20]));\
	a8  = ((ccClip255[(a3 + a1)>>20]));\
	a3  = (ccClip255[(a3 - a0)>>20]);\
	a5  = ((a5<<16)|(a3<<8)|a8);\
	a5 |= (alpha_value<< 24) ;\
}

#define WRITE_PIX(a4, x){\
	*((VO_S32*)(out_buf+(x))) = (VO_S32)a4;\
}

#define WRITE90L_PIX(a4, x){\
	*((VO_S32*)(out_buf+(x))) = (VO_S32)a4;\
	out_buf +=(x);\
}

#define WRITE90R_PIX(a4, x){\
	*((VO_S32*)(out_buf+(x))) = (VO_S32)a4;\
	out_buf +=(x);\
}

#define WRITERGB32_PIX(a4,a5, x){\
	*((VO_S32*)(out_buf+(x)))   = (VO_S32)a4;\
	*((VO_S32*)(out_buf+(x)+4)) = (VO_S32)a5;\
}

#define WRITERGB32_90L_PIX(a4,a5, x){\
	*((VO_S32*)(out_buf+(x)))   = (VO_S32)a4;\
	*((VO_S32*)(out_buf+(x)+4)) = (VO_S32)a5;\
	out_buf +=(x);\
}

#define WRITERGB32_90R_PIX(a4,a5, x){\
	*((VO_S32*)(out_buf+(x)+4))   = (VO_S32)a4;\
	*((VO_S32*)(out_buf+(x))) = (VO_S32)a5;\
	out_buf +=(x);\
}

void cc_mb_16x16_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
				   VO_S32 width, VO_S32 height, const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5,a7;
		//0
		NEWLOAD_DATA(a3, a7, 0, 0)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 1)
		NEWPIX_1(a3)

		WRITE_PIX(a4, 0)
		NEWPIX_2(a7)
		NEWLOAD_YPIX(a7, in_stride+1)
		NEWPIX_3(a7)

		WRITE_PIX(a4, out_stride)

		//1	
		NEWLOAD_DATA(a3, a7, 1, 2)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 3)	
		NEWPIX_1(a3)

		WRITE_PIX(a4, 4)

		NEWPIX_2(a7)
		NEWLOAD_YPIX(a7, in_stride+3)
		NEWPIX_3(a7)

		WRITE_PIX(a4, out_stride+4)

		//2
		NEWLOAD_DATA(a3, a7, 2, 4)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 5)
		NEWPIX_1(a3)

		WRITE_PIX(a4, 8)

		NEWPIX_2(a7)
		NEWLOAD_YPIX(a7, in_stride+5)
		NEWPIX_3(a7)

		WRITE_PIX(a4, out_stride+8)

		//3
		NEWLOAD_DATA(a3, a7, 3, 6)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 7)	
		NEWPIX_1(a3)

		WRITE_PIX(a4, 12)

		NEWPIX_2(a7)
		NEWLOAD_YPIX(a7, in_stride+7)
		NEWPIX_3(a7)

		WRITE_PIX(a4, out_stride+12)

		//4
		NEWLOAD_DATA(a3, a7, 4, 8)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 9)	
		NEWPIX_1(a3)

		WRITE_PIX(a4, 16)

		NEWPIX_2(a7)
		NEWLOAD_YPIX(a7, in_stride+9)
		NEWPIX_3(a7)

		WRITE_PIX(a4, out_stride+16)

		//5
		NEWLOAD_DATA(a3, a7, 5, 10)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 11)	
		NEWPIX_1(a3)

		WRITE_PIX(a4, 20)

		NEWPIX_2(a7)
		NEWLOAD_YPIX(a7, in_stride+11)
		NEWPIX_3(a7)

		WRITE_PIX(a4, out_stride+20)

		//6
		NEWLOAD_DATA(a3, a7, 6, 12)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 13)	
		NEWPIX_1(a3)

		WRITE_PIX(a4, 24)

		NEWPIX_2(a7)
		NEWLOAD_YPIX(a7, in_stride+13)
		NEWPIX_3(a7)

		WRITE_PIX(a4, out_stride+24)

		//7
		NEWLOAD_DATA(a3, a7, 7, 14)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 15)	
		NEWPIX_1(a3)

		WRITE_PIX(a4, 28)

		NEWPIX_2(a7)
		NEWLOAD_YPIX(a7, in_stride+15)
		NEWPIX_3(a7)
		WRITE_PIX(a4, out_stride+28)

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf += (out_stride<<1);

	}while(--a6 != 0);
}

//void cc_mb(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride,VO_U8 *dst, const VO_S32 dst_stride,
//		   VO_S32 width, VO_S32 height, const VO_S32 uin_stride, const VO_S32 vin_stride)
//{
//	VO_S32 i;
//
//	do{
//		i = width;
//		do{
//			VO_S32 a0, a1, a2, a3, a4;
//
//			a3 = *(src_v++) - 128;	
//			a2 = *(src_u++) - 128;
//
//			a0 = a3 * ConstV1;
//			a1 = a3 * ConstV2 + a2 *ConstU2;
//			a2 = a2 * ConstU1;
//			a3 = ((*src_y) - 16) * ConstY;
//			a4 = (*(src_y + 1) - 16) * ConstY;
//
//			a3 = ((ccClip255[((a3 + a0)>>20) + RDITHERNEW_P0]>>3)<<11)|((ccClip255[((a3 - a1)>>20) + GDITHERNEW_P0]>>2)<<5)|(ccClip255[((a3 + a2)>>20) + BDITHERNEW_P0]>>3);
//			a4 = ((ccClip255[((a4 + a0)>>20) + RDITHERNEW_P1]>>3)<<11)|((ccClip255[((a4 - a1)>>20) + GDITHERNEW_P1]>>2)<<5)|(ccClip255[((a4 + a2)>>20) + BDITHERNEW_P1]>>3);
//			a4 = (a4 << 16) | a3;
//			*((VO_U32*)dst) = a4;
//
//			/////////////////////////////////////////////////////////////////////
//			a3 = (*(src_y + src_stride) - 16) * ConstY;
//			a4 = (*(src_y + src_stride + 1) - 16) * ConstY;
//			src_y += 2;
//
//			a3 = ((ccClip255[((a3 + a0)>>20) + RDITHERNEW_P2]>>3)<<11)|((ccClip255[((a3 - a1)>>20) + GDITHERNEW_P2]>>2)<<5)|(ccClip255[((a3 + a2)>>20) + BDITHERNEW_P2]>>3);
//			a4 = ((ccClip255[((a4 + a0)>>20) + RDITHERNEW_P3]>>3)<<11)|((ccClip255[((a4 - a1)>>20) + GDITHERNEW_P3]>>2)<<5)|(ccClip255[((a4 + a2)>>20) + BDITHERNEW_P3]>>3);
//			a4 = (a4 << 16) | a3;
//			*((VO_U32*)(dst+dst_stride)) = a4;
//			dst += 4;
//
//		}while((i-=2) != 0);
//
//		i = (width >> 1);
//		src_u -= i;
//		src_v -= i;
//		//i = (src_stride>>1);
//		src_u += uin_stride;
//		src_v += vin_stride;
//		src_y -= width;
//		src_y += (src_stride << 1);
//
//		dst -= (width <<1 );
//		dst += (dst_stride << 1);
//	}while((height-=2) != 0);
//}

//huwei 20110530 bug fixed
void cc_mb(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride,VO_U8 *dst, const VO_S32 dst_stride,
		   VO_S32 width, VO_S32 height, const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;

	do{
		i = width;
		do{
			VO_S32 a0, a1, a2, a3, a4;

			a3 = *(src_v++) - 128;	
			a2 = *(src_u++) - 128;

			a0 = a3 * ConstV1;
			a1 = a3 * ConstV2 + a2 *ConstU2;
			a2 = a2 * ConstU1;
			a3 = ((*src_y) - 16) * ConstY;
			a4 = (*(src_y + 1) - 16) * ConstY;

			a3 = ((ccClip63[((a3 + a0)>>22)]>>1)<<11)|(ccClip63[((a3 - a1)>>22)]<<5)|(ccClip63[((a3 + a2)>>22)]>>1);
			a4 = ((ccClip63[((a4 + a0)>>22)]>>1)<<11)|(ccClip63[((a4 - a1)>>22)]<<5)|(ccClip63[((a4 + a2)>>22)]>>1);
			a4 = (a4 << 16) | a3;
			*((VO_U32*)dst) = a4;

			/////////////////////////////////////////////////////////////////////
			a3 = (*(src_y + src_stride) - 16) * ConstY;
			a4 = (*(src_y + src_stride + 1) - 16) * ConstY;
			src_y += 2;

			a3 = ((ccClip63[((a3 + a0)>>22)]>>1)<<11)|(ccClip63[((a3 - a1)>>22)]<<5)|(ccClip63[((a3 + a2)>>22)]>>1);
			a4 = ((ccClip63[((a4 + a0)>>22)]>>1)<<11)|(ccClip63[((a4 - a1)>>22)]<<5)|(ccClip63[((a4 + a2)>>22)]>>1);
			a4 = (a4 << 16) | a3;
			*((VO_U32*)(dst+dst_stride)) = a4;
			dst += 4;

		}while((i-=2) != 0);

		i = (width >> 1);
		src_u -= i;
		src_v -= i;
		//i = (src_stride>>1);
		src_u += uin_stride;
		src_v += vin_stride;
		src_y -= width;
		src_y += (src_stride << 1);

		dst -= (width <<1 );
		dst += (dst_stride << 1);
	}while((height-=2) != 0);
}

void cc_rgb32_mb_16x16_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
						 VO_S32 width, VO_S32 height, const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5,a7,a8;
		//0
		NEWLOAD_DATA(a3, a7, 0, 0)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 1)
		NEWPIXRGB32_1(a3)

		WRITERGB32_PIX(a4,a5,0)
		NEWPIXRGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+1)
		NEWPIXRGB32_3(a7)

		WRITERGB32_PIX(a4,a5,out_stride)

		//1	
		NEWLOAD_DATA(a3, a7, 1, 2)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 3)	
		NEWPIXRGB32_1(a3)

		WRITERGB32_PIX(a4,a5, 8)

		NEWPIXRGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+3)
		NEWPIXRGB32_3(a7)

		WRITERGB32_PIX(a4,a5,out_stride+8)

		//2
		NEWLOAD_DATA(a3, a7, 2, 4)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 5)
		NEWPIXRGB32_1(a3)

		WRITERGB32_PIX(a4,a5, 16)

		NEWPIXRGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+5)
		NEWPIXRGB32_3(a7)

		WRITERGB32_PIX(a4,a5, out_stride+16)

		//3
		NEWLOAD_DATA(a3, a7, 3, 6)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 7)	
		NEWPIXRGB32_1(a3)

		WRITERGB32_PIX(a4,a5, 24)

		NEWPIXRGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+7)
		NEWPIXRGB32_3(a7)

		WRITERGB32_PIX(a4,a5, out_stride+24)

		//4
		NEWLOAD_DATA(a3, a7, 4, 8)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 9)	
		NEWPIXRGB32_1(a3)

		WRITERGB32_PIX(a4,a5, 32)

		NEWPIXRGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+9)
		NEWPIXRGB32_3(a7)

		WRITERGB32_PIX(a4,a5, out_stride+32)

		//5
		NEWLOAD_DATA(a3, a7, 5, 10)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 11)	
		NEWPIXRGB32_1(a3)

		WRITERGB32_PIX(a4,a5, 40)

		NEWPIXRGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+11)
		NEWPIXRGB32_3(a7)

		WRITERGB32_PIX(a4,a5, out_stride+40)

		//6
		NEWLOAD_DATA(a3, a7, 6, 12)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 13)	
		NEWPIXRGB32_1(a3)

		WRITERGB32_PIX(a4,a5, 48)

		NEWPIXRGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+13)
		NEWPIXRGB32_3(a7)

		WRITERGB32_PIX(a4,a5, out_stride+48)

		//7
		NEWLOAD_DATA(a3, a7, 7, 14)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 15)	
		NEWPIXRGB32_1(a3)

		WRITERGB32_PIX(a4,a5, 56)

		NEWPIXRGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+15)
		NEWPIXRGB32_3(a7)
		WRITERGB32_PIX(a4,a5, out_stride+56)

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf += (out_stride<<1);

	}while(--a6 != 0);
}

void cc_rgb32_mb(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride,VO_U8 *dst, const VO_S32 dst_stride,
				 VO_S32 width, VO_S32 height, const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;

	do{
		i = width;
		do{
			VO_S32 a0, a1, a2, a3, a4;

			a3 = *(src_v++) - 128;	
			a2 = *(src_u++) - 128;

			a0 = a3 * ConstV1;
			a1 = a3 * ConstV2 + a2 *ConstU2;
			a2 = a2 * ConstU1;
			a3 = ((*src_y) - 16) * ConstY;
			a4 = (*(src_y + 1) - 16) * ConstY;

			a3 = ((ccClip255[((a3 + a0)>>20) + RDITHERNEW_P0])<<16)|((ccClip255[((a3 - a1)>>20) + GDITHERNEW_P0])<<8)|(ccClip255[((a3 + a2)>>20) + BDITHERNEW_P0]);
			a4 = ((ccClip255[((a4 + a0)>>20) + RDITHERNEW_P1])<<16)|((ccClip255[((a4 - a1)>>20) + GDITHERNEW_P1])<<8)|(ccClip255[((a4 + a2)>>20) + BDITHERNEW_P1]);
			*((VO_U32*)dst)       = a3;
			*((VO_U32*)(dst + 4)) = a4;

			/////////////////////////////////////////////////////////////////////
			a3 = (*(src_y + src_stride) - 16) * ConstY;
			a4 = (*(src_y + src_stride + 1) - 16) * ConstY;
			src_y += 2;

			a3 = ((ccClip255[((a3 + a0)>>20) + RDITHERNEW_P2])<<16)|((ccClip255[((a3 - a1)>>20) + GDITHERNEW_P2])<<8)|(ccClip255[((a3 + a2)>>20) + BDITHERNEW_P2]);
			a4 = ((ccClip255[((a4 + a0)>>20) + RDITHERNEW_P3])<<16)|((ccClip255[((a4 - a1)>>20) + GDITHERNEW_P3])<<8)|(ccClip255[((a4 + a2)>>20) + BDITHERNEW_P3]);
			*((VO_U32*)(dst+dst_stride))     = a3;
			*((VO_U32*)(dst+dst_stride + 4)) = a4;
			dst += 8;

		}while((i-=2) != 0);

		i = (width >> 1);
		src_u -= i;
		src_v -= i;
		src_u += uin_stride;
		src_v += vin_stride;
		src_y -= width;
		src_y += (src_stride << 1);

		dst -= (width << 2 );
		dst += (dst_stride << 1);
	}while((height-=2) != 0);
}

void cc_rgb32_mb_new(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride,VO_U8 *dst, const VO_S32 dst_stride,
					 VO_S32 width, VO_S32 height, const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	VO_S32 b0, b1, b2;

	do{
		i = width;
		do{
			VO_S32 a0, a1, a2, a3, a4;

			a3 = *(src_v++) - 128;	
			a2 = *(src_u++) - 128;

			a0 = a3 * ARMV7ConstV1;
			a1 = a3 * ARMV7ConstV2 + a2 *ARMV7ConstU2;
			a2 = a2 * ARMV7ConstU1;
			a3 = ((*src_y) - 16) * ARMV7ConstY;
			a4 = (*(src_y + 1) - 16) * ARMV7ConstY;

			b0 = SAT((a3 + a0)>>12)
			b1 = SAT((a3 - a1)>>12)
			b2 = SAT((a3 + a2)>>12)
			a3 = (b0<<16)|(b1<<8)|b2;
			b0 = SAT((a4 + a0)>>12)
			b1 = SAT((a4 - a1)>>12)
			b2 = SAT((a4 + a2)>>12)
			a4 = (b0<<16)|(b1<<8)|b2;

			*((VO_U32*)dst)       = a3;
			*((VO_U32*)(dst + 4)) = a4;

			/////////////////////////////////////////////////////////////////////
			a3 = (*(src_y + src_stride) - 16) * ARMV7ConstY;
			a4 = (*(src_y + src_stride + 1) - 16) * ARMV7ConstY;
			src_y += 2;

			b0 = SAT((a3 + a0)>>12)
			b1 = SAT((a3 - a1)>>12)
			b2 = SAT((a3 + a2)>>12)
			a3 = (b0<<16)|(b1<<8)|b2;
			b0 = SAT((a4 + a0)>>12)
			b1 = SAT((a4 - a1)>>12)
			b2 = SAT((a4 + a2)>>12)
			a4 = (b0<<16)|(b1<<8)|b2;
			*((VO_U32*)(dst+dst_stride))     = a3;
			*((VO_U32*)(dst+dst_stride + 4)) = a4;
			dst += 8;

		}while((i-=2) != 0);

		i = (width >> 1);
		src_u -= i;
		src_v -= i;
		src_u += uin_stride;
		src_v += vin_stride;
		src_y -= width;
		src_y += (src_stride << 1);

		dst -= (width << 2 );
		dst += (dst_stride << 1);
	}while((height-=2) != 0);
}

void cc_argb32_mb_16x16_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
						 VO_S32 width, VO_S32 height, const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5,a7,a8;
		//0
		NEWLOAD_DATA(a3, a7, 0, 0)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 1)
		NEWPIXARGB32_1(a3)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5,0)
		NEWPIXARGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+1)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5,out_stride)

		//1	
		NEWLOAD_DATA(a3, a7, 1, 2)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 3)	
		NEWPIXARGB32_1(a3)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5, 8)

		NEWPIXARGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+3)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5,out_stride+8)

		//2
		NEWLOAD_DATA(a3, a7, 2, 4)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 5)
		NEWPIXARGB32_1(a3)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5, 16)

		NEWPIXARGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+5)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5, out_stride+16)

		//3
		NEWLOAD_DATA(a3, a7, 3, 6)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 7)	
		NEWPIXARGB32_1(a3)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5, 24)

		NEWPIXARGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+7)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5, out_stride+24)

		//4
		NEWLOAD_DATA(a3, a7, 4, 8)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 9)	
		NEWPIXARGB32_1(a3)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5, 32)

		NEWPIXARGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+9)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5, out_stride+32)

		//5
		NEWLOAD_DATA(a3, a7, 5, 10)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 11)	
		NEWPIXARGB32_1(a3)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5, 40)

		NEWPIXARGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+11)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5, out_stride+40)

		//6
		NEWLOAD_DATA(a3, a7, 6, 12)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 13)	
		NEWPIXARGB32_1(a3)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5, 48)

		NEWPIXARGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+13)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5, out_stride+48)

		//7
		NEWLOAD_DATA(a3, a7, 7, 14)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 15)	
		NEWPIXARGB32_1(a3)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5, 56)

		NEWPIXARGB32_2(a7)
		NEWLOAD_YPIX(a7, in_stride+15)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_PIX(a4,a5, out_stride+56)

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf += (out_stride<<1);

	}while(--a6 != 0);
}

void cc_argb32_mb(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride,VO_U8 *dst, const VO_S32 dst_stride,
				 VO_S32 width, VO_S32 height, const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;

	do{
		i = width;
		do{
			VO_S32 a0, a1, a2, a3, a4;

			a3 = *(src_v++) - 128;	
			a2 = *(src_u++) - 128;

			a0 = a3 * ConstV1;
			a1 = a3 * ConstV2 + a2 *ConstU2;
			a2 = a2 * ConstU1;
			a3 = ((*src_y) - 16) * ConstY;
			a4 = (*(src_y + 1) - 16) * ConstY;

			a3 = ((ccClip255[((a3 + a0)>>20) + RDITHERNEW_P0]))|((ccClip255[((a3 - a1)>>20) + GDITHERNEW_P0])<<8)|((ccClip255[((a3 + a2)>>20) + BDITHERNEW_P0])<<16)|(255 << 24);
			a4 = ((ccClip255[((a4 + a0)>>20) + RDITHERNEW_P1]))|((ccClip255[((a4 - a1)>>20) + GDITHERNEW_P1])<<8)|((ccClip255[((a4 + a2)>>20) + BDITHERNEW_P1])<<16)|(255 << 24);
			*((VO_U32*)dst)       = a3;
			*((VO_U32*)(dst + 4)) = a4;

			/////////////////////////////////////////////////////////////////////
			a3 = (*(src_y + src_stride) - 16) * ConstY;
			a4 = (*(src_y + src_stride + 1) - 16) * ConstY;
			src_y += 2;

			a3 = ((ccClip255[((a3 + a0)>>20) + RDITHERNEW_P2]))|((ccClip255[((a3 - a1)>>20) + GDITHERNEW_P2])<<8)|((ccClip255[((a3 + a2)>>20) + BDITHERNEW_P2])<<16)|(255 << 24);
			a4 = ((ccClip255[((a4 + a0)>>20) + RDITHERNEW_P3]))|((ccClip255[((a4 - a1)>>20) + GDITHERNEW_P3])<<8)|((ccClip255[((a4 + a2)>>20) + BDITHERNEW_P3])<<16)|(255 << 24);
			*((VO_U32*)(dst+dst_stride))     = a3;
			*((VO_U32*)(dst+dst_stride + 4)) = a4;
			dst += 8;

		}while((i-=2) != 0);

		i = (width >> 1);
		src_u -= i;
		src_v -= i;
		src_u += uin_stride;
		src_v += vin_stride;
		src_y -= width;
		src_y += (src_stride << 1);

		dst -= (width << 2 );
		dst += (dst_stride << 1);
	}while((height-=2) != 0);
}

void cc_argb32_mb_new(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride,VO_U8 *dst, const VO_S32 dst_stride,
					  VO_S32 width, VO_S32 height, const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	VO_S32 b0, b1, b2;

	do{
		i = width;
		do{
			VO_S32 a0, a1, a2, a3, a4;

			a3 = *(src_v++) - 128;	
			a2 = *(src_u++) - 128;

			a0 = a3 * ARMV7ConstV1;
			a1 = a3 * ARMV7ConstV2 + a2 *ARMV7ConstU2;
			a2 = a2 * ARMV7ConstU1;
			a3 = ((*src_y) - 16) * ARMV7ConstY;
			a4 = (*(src_y + 1) - 16) * ARMV7ConstY;

			b0 = SAT((a3 + a0)>>12)
			b1 = SAT((a3 - a1)>>12)
			b2 = SAT((a3 + a2)>>12)
			a3 = (255 << 24)|(b2<<16)|(b1<<8)|b0;
			b0 = SAT((a4 + a0)>>12)
			b1 = SAT((a4 - a1)>>12)
			b2 = SAT((a4 + a2)>>12)
			a4 = (255 << 24)|(b2<<16)|(b1<<8)|b0;

			*((VO_U32*)dst)       = a3;
			*((VO_U32*)(dst + 4)) = a4;

			/////////////////////////////////////////////////////////////////////
			a3 = (*(src_y + src_stride) - 16) * ARMV7ConstY;
			a4 = (*(src_y + src_stride + 1) - 16) * ARMV7ConstY;
			src_y += 2;

			b0 = SAT((a3 + a0)>>12)
			b1 = SAT((a3 - a1)>>12)
			b2 = SAT((a3 + a2)>>12)
			a3 = (255 << 24)|(b2<<16)|(b1<<8)|b0;
			b0 = SAT((a4 + a0)>>12)
			b1 = SAT((a4 - a1)>>12)
			b2 = SAT((a4 + a2)>>12)
			a4 = (255 << 24)|(b2<<16)|(b1<<8)|b0;
			*((VO_U32*)(dst+dst_stride))     = a3;
			*((VO_U32*)(dst+dst_stride + 4)) = a4;
			dst += 8;

		}while((i-=2) != 0);

		i = (width >> 1);
		src_u -= i;
		src_v -= i;
		src_u += uin_stride;
		src_v += vin_stride;
		src_y -= width;
		src_y += (src_stride << 1);

		dst -= (width << 2 );
		dst += (dst_stride << 1);
	}while((height-=2) != 0);

}

void yuv420_rgb24_mb_16x16_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stridey,VO_U8 *out_buf, const VO_S32 out_stride,
							 VO_S32 width, VO_S32 height, const VO_S32 in_strideu, const VO_S32 in_stridev)
{
	VO_S32 a0, a1, a2, a3, a4, a5, a7, a33, a77;
	VO_S32 i, j;
	for(i = 0; i < height; i += 2){

		for(j = 0; j < width; j += 2){
			a0 = psrc_u[0] - 128;
			a1 = psrc_v[0] - 128;
			a2 = (a0 * ConstU1_16BIT);
			a0 = (a1 * ConstV2_16BIT + a0 *ConstU2_16BIT);
			a1 = (a1 * ConstV1_16BIT);

			a3 = (psrc_y[0] - 16)*ConstY_16BIT;
			a33 = (psrc_y[1] - 16)*ConstY_16BIT;

			a7 = (psrc_y[in_stridey+0] - 16)*ConstY_16BIT;
			a77 = (psrc_y[in_stridey+1] - 16)*ConstY_16BIT;

			a4  = SAT((a3 + a1)>>12);
			a5  = SAT((a3 - a0)>>12);
			a3  = SAT((a3 + a2)>>12);
			*(out_buf) = (VO_U8)a3;
			*(out_buf+1) = (VO_U8)a5;
			*(out_buf+2) = (VO_U8)a4;

			a4  = SAT((a33 + a1)>>12);
			a5  = SAT((a33 - a0)>>12);
			a33  = SAT((a33 + a2)>>12);
			*(out_buf+3) = (VO_U8)a33;
			*(out_buf+4) = (VO_U8)a5;
			*(out_buf+5) = (VO_U8)a4;

			a4  = SAT((a7 + a1)>>12);
			a5  = SAT((a7 - a0)>>12);
			a7  = SAT((a7 + a2)>>12);
			*(out_buf+0+out_stride) = (VO_U8)a7;
			*(out_buf+1+out_stride) = (VO_U8)a5;
			*(out_buf+2+out_stride) = (VO_U8)a4;

			a4  = SAT((a77 + a1)>>12);
			a5  = SAT((a77 - a0)>>12);
			a77  = SAT((a77 + a2)>>12);
			*(out_buf+3+out_stride) = (VO_U8)a77;
			*(out_buf+4+out_stride) = (VO_U8)a5;
			*(out_buf+5+out_stride) = (VO_U8)a4;

			out_buf += 6;
			psrc_y += 2;
			psrc_u++;
			psrc_v++;

		}
		psrc_y -= width;
		psrc_y += (in_stridey<<1);

		psrc_u -= (width>>1);
		psrc_u += in_strideu;

		psrc_v -= (width>>1);
		psrc_v += in_stridev;

		out_buf -= (width*3);
		out_buf += (out_stride<<1);
	}
}

void yuv420_rgb24(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stridey,VO_U8 *out_buf, const VO_S32 out_stride,
				  VO_S32 width, VO_S32 height, const VO_S32 in_strideu,const VO_S32 in_stridev)
{
	VO_S32 a0, a1, a2, a3, a4, a5, a7, a33, a77;
	VO_S32 i, j;
	for(i = 0; i < height; i += 2){

		for(j = 0; j < width; j += 2){
			a0 = psrc_u[0] - 128;
			a1 = psrc_v[0] - 128;
			a2 = (a0 * ConstU1_16BIT);
			a0 = (a1 * ConstV2_16BIT + a0 *ConstU2_16BIT);
			a1 = (a1 * ConstV1_16BIT);

			a3 = (psrc_y[0] - 16)*ConstY_16BIT;
			a33 = (psrc_y[1] - 16)*ConstY_16BIT;

			a7 = (psrc_y[in_stridey+0] - 16)*ConstY_16BIT;
			a77 = (psrc_y[in_stridey+1] - 16)*ConstY_16BIT;

			a4  = SAT((a3 + a1)>>12);
			a5  = SAT((a3 - a0)>>12);
			a3  = SAT((a3 + a2)>>12);
			*(out_buf) = (VO_U8)a3;
			*(out_buf+1) = (VO_U8)a5;
			*(out_buf+2) = (VO_U8)a4;

			a4  = SAT((a33 + a1)>>12);
			a5  = SAT((a33 - a0)>>12);
			a33  = SAT((a33 + a2)>>12);
			*(out_buf+3) = (VO_U8)a33;
			*(out_buf+4) = (VO_U8)a5;
			*(out_buf+5) = (VO_U8)a4;

			a4  = SAT((a7 + a1)>>12);
			a5  = SAT((a7 - a0)>>12);
			a7  = SAT((a7 + a2)>>12);
			*(out_buf+0+out_stride) = (VO_U8)a7;
			*(out_buf+1+out_stride) = (VO_U8)a5;
			*(out_buf+2+out_stride) = (VO_U8)a4;

			a4  = SAT((a77 + a1)>>12);
			a5  = SAT((a77 - a0)>>12);
			a77  = SAT((a77 + a2)>>12);
			*(out_buf+3+out_stride) = (VO_U8)a77;
			*(out_buf+4+out_stride) = (VO_U8)a5;
			*(out_buf+5+out_stride) = (VO_U8)a4;

			out_buf += 6;
			psrc_y += 2;
			psrc_u++;
			psrc_v++;

		}
		psrc_y -= width;
		psrc_y += (in_stridey<<1);

		psrc_u -= (width>>1);
		psrc_u += in_strideu;

		psrc_v -= (width>>1);
		psrc_v += in_stridev;

		out_buf -= (width*3);
		out_buf += (out_stride<<1);
	}
}

#ifdef COLCONVENABLE
void YUV444_to_RGB565_cc_mb_16x16_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
									VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
									const VO_S32 uin_stride, const VO_S32 vin_stride, VO_S32 * const param_tab)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5,a7;
		//0
		LOAD_DATA(a3, a7, 0, 0)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 1)
		LOAD_UPIX(a0, 1)
		LOAD_VPIX(a1, 1)
		CAL_UV()
		PIX_1(a3)

		WRITE_PIX(a4, 0)
		LOAD_UPIX(a0, in_stride)
		LOAD_VPIX(a1, in_stride)
		CAL_UV()
		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 1)
		LOAD_UPIX(a0, in_stride + 1)
		LOAD_VPIX(a1, in_stride + 1)
		CAL_UV()
		PIX_3(a7)

		WRITE_PIX(a4, out_stride)

		//1	
		LOAD_DATA(a3, a7, 2, 2)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 3)
		LOAD_UPIX(a0, 3)
		LOAD_VPIX(a1, 3)
		CAL_UV()
		PIX_1(a3)

		WRITE_PIX(a4, 4)

		LOAD_UPIX(a0, in_stride + 2)
		LOAD_VPIX(a1, in_stride + 2)
		CAL_UV()
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 3)
		LOAD_UPIX(a0, in_stride + 3)
		LOAD_VPIX(a1, in_stride + 3)
		CAL_UV()
		PIX_3(a7)

		WRITE_PIX(a4, out_stride + 4)

		//2
		LOAD_DATA(a3, a7, 4, 4)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 5)
		LOAD_UPIX(a0, 5)
		LOAD_VPIX(a1, 5)
		CAL_UV()
		PIX_1(a3)

		WRITE_PIX(a4, 8)

		LOAD_UPIX(a0, in_stride + 4)
		LOAD_VPIX(a1, in_stride + 4)
		CAL_UV()
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 5)
		LOAD_UPIX(a0, in_stride + 5)
		LOAD_VPIX(a1, in_stride + 5)
		CAL_UV()
		PIX_3(a7)

		WRITE_PIX(a4, out_stride+8)

		//3
		LOAD_DATA(a3, a7, 6, 6)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 7)
		LOAD_UPIX(a0, 7)
		LOAD_VPIX(a1, 7)
		CAL_UV()
		PIX_1(a3)

		WRITE_PIX(a4, 12)

		PIX_2(a7)
		LOAD_UPIX(a0, in_stride + 6)
		LOAD_VPIX(a1, in_stride + 6)
		CAL_UV()
		LOAD_YPIX(a7, in_stride + 7)
		LOAD_UPIX(a0, in_stride + 7)
		LOAD_VPIX(a1, in_stride + 7)
		CAL_UV()
		PIX_3(a7)

		WRITE_PIX(a4, out_stride+12)

		//4
		LOAD_DATA(a3, a7, 8, 8)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 9)
		LOAD_UPIX(a0, 9)
		LOAD_VPIX(a1, 9)
		CAL_UV()
		PIX_1(a3)

		WRITE_PIX(a4, 16)

		LOAD_UPIX(a0, in_stride + 8)
		LOAD_VPIX(a1, in_stride + 8)
		CAL_UV()
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 9)
		LOAD_UPIX(a0, in_stride + 9)
		LOAD_VPIX(a1, in_stride + 9)
		CAL_UV()
		PIX_3(a7)

		WRITE_PIX(a4, out_stride+16)

		//5
		LOAD_DATA(a3, a7, 10, 10)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 11)
		LOAD_UPIX(a0, 11)
		LOAD_VPIX(a1, 11)
		CAL_UV()
		PIX_1(a3)

		WRITE_PIX(a4, 20)

		LOAD_UPIX(a0, in_stride + 10)
		LOAD_VPIX(a1, in_stride + 10)
		CAL_UV()
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 11)
		LOAD_UPIX(a0, in_stride + 11)
		LOAD_VPIX(a1, in_stride + 11)
		CAL_UV()
		PIX_3(a7)

		WRITE_PIX(a4, out_stride+20)

		//6
		LOAD_DATA(a3, a7, 12, 12)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 13)
		LOAD_UPIX(a0, 13)
		LOAD_VPIX(a1, 13)
		CAL_UV()
		PIX_1(a3)

		WRITE_PIX(a4, 24)

		LOAD_UPIX(a0, in_stride + 12)
		LOAD_VPIX(a1, in_stride + 12)
		CAL_UV()
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 13)
		LOAD_UPIX(a0, in_stride + 13)
		LOAD_VPIX(a1, in_stride + 13)
		CAL_UV()
		PIX_3(a7)

		WRITE_PIX(a4, out_stride + 24)

		//7
		LOAD_DATA(a3, a7, 14, 14)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 15)
		LOAD_UPIX(a0, 15)
		LOAD_VPIX(a1, 15)
		CAL_UV()
		PIX_1(a3)

		WRITE_PIX(a4, 28)

		LOAD_UPIX(a0, in_stride + 14)
		LOAD_VPIX(a1, in_stride + 14)
		CAL_UV()
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 15)
		LOAD_UPIX(a0, in_stride + 15)
		LOAD_VPIX(a1, in_stride + 15)
		CAL_UV()
		PIX_3(a7)
		WRITE_PIX(a4, out_stride+28)

		psrc_y += (in_stride<<1);
		psrc_u += (in_stride<<1);
		psrc_v += (in_stride<<1);
		out_buf += (out_stride<<1);

	}while(--a6 != 0);

}

void YUV444_to_RGB565_cc_mb(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride, 
							VO_U8 *dst, const VO_S32 dst_stride, VO_S32 width, VO_S32 height,
							const VO_S32 uin_stride, const VO_S32 vin_stride, VO_S32* const param_tab)
{
	VO_U32 vx = 0, vy;
	const VO_U32 in_stride = src_stride , out_width = width, out_height = height;
	VO_U8 *outbuf, *start_outbuf;
	VO_U8 *psrcY, *psrcU, *psrcV;
	VO_S32 *dithr, *dithg, *dithb;

	VO_S32 step_y, step_x, out_stride = dst_stride;

	/*start of output address*/
	step_y = 2;
	step_x = out_stride;
	start_outbuf = dst;

	for(vy = 0; vy < out_height; vy++) {
		outbuf = start_outbuf;
		vx =  vy * in_stride;
		psrcY = src_y + vx;
		psrcU = src_u + vx;
		psrcV = src_v + vx;

		dithr = ditherrb[vy&1];
		dithb = ditherrb[(vy+1)&1];
		dithg = ditherg[vy&1];

		for(vx = 0; vx < out_width; vx++) {
			int a0, a1, a2, a3;

			a3 = psrcV[vx] - 128;
			a2 = psrcU[vx] - 128;

			a0 = (a3 * param_tab[0]) >> 20;
			a1 = (a3 * param_tab[1] + a2 *param_tab[3]) >> 20;
			a3 = psrcY[vx];
			a2 = (a2 * param_tab[2]) >> 20;

			a0  = ccClip31[(a3 + a0 + dithr[vx&1])>>3];
			a1  = ccClip63[(a3 - a1 + dithg[vx&1])>>2];
			a2  = ccClip31[(a3 + a2 + dithb[vx&1])>>3];

			a0 = ((((a0<<6)|a1 )<<5))|a2;

			*((short*)outbuf) = (unsigned short)a0;
			outbuf += step_y;
		}
		start_outbuf += step_x;
	}	
}

void YUV422_21_to_RGB565_cc_mb_16x16_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
								   VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
								   const VO_S32 uin_stride, const VO_S32 vin_stride, VO_S32* const param_tab)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5,a7;

		//0
		LOAD_DATA(a3, a7, 0, 0)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 1)
		PIX_1(a3)

		WRITE_PIX(a4, 0)
		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 1)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride)

		//1	
		LOAD_DATA(a3, a7, 2, 2)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 3)
		PIX_1(a3)

		WRITE_PIX(a4, 4)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 3)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride + 4)

		//2
		LOAD_DATA(a3, a7, 4, 4)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 5)
		PIX_1(a3)

		WRITE_PIX(a4, 8)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 5)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride+8)

		//3
		LOAD_DATA(a3, a7, 6, 6)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 7)
		PIX_1(a3)

		WRITE_PIX(a4, 12)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 7)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride+12)

		//4
		LOAD_DATA(a3, a7, 8, 8)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 9)
		PIX_1(a3)

		WRITE_PIX(a4, 16)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 9)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride+16)

		//5
		LOAD_DATA(a3, a7, 10, 10)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 11)
		PIX_1(a3)

		WRITE_PIX(a4, 20)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 11)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride+20)

		//6
		LOAD_DATA(a3, a7, 12, 12)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 13)
		PIX_1(a3)

		WRITE_PIX(a4, 24)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 13)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride + 24)

		//7
		LOAD_DATA(a3, a7, 14, 14)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 15)
		PIX_1(a3)

		WRITE_PIX(a4, 28)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 15)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride+28)

		psrc_y += (in_stride<<1);
		psrc_u += in_stride;
		psrc_v += in_stride;
		out_buf += (out_stride<<1);

	}while(--a6 != 0);

}

void YUV422_21_to_RGB565_cc_mb(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride, 
							   VO_U8 *dst, const VO_S32 dst_stride, VO_S32 width, VO_S32 height,
							   const VO_S32 uin_stride, const VO_S32 vin_stride, VO_S32* const param_tab)
{
	VO_U32 vx = 0, vy;
	const VO_U32 in_stride = src_stride , out_width = width, out_height = height;
	VO_U8 *outbuf, *start_outbuf;
	VO_U8 *psrcY, *psrcU, *psrcV;
	VO_S32 *dithr, *dithg, *dithb;

	VO_S32 step_y, step_x, out_stride = dst_stride;

	/*start of output address*/
	step_y = 2;
	step_x = out_stride;
	start_outbuf = dst;

	for(vy = 0; vy < out_height; vy++) {
		outbuf = start_outbuf;
		vx =  vy * in_stride;
		psrcY = src_y + vx;
		vx =  (vy>>1) * in_stride;
		psrcU = src_u + vx;
		psrcV = src_v + vx;

		dithr = ditherrb[vy&1];
		dithb = ditherrb[(vy+1)&1];
		dithg = ditherg[vy&1];

		for(vx = 0; vx < out_width; vx++) {
			int a0, a1, a2, a3;

			a3 = psrcV[(vx>>1)<<1] - 128;
			a2 = psrcU[(vx>>1)<<1] - 128;

			a0 = (a3 * param_tab[0]) >> 20;
			a1 = (a3 * param_tab[1] + a2 *param_tab[3]) >> 20;
			a3 = psrcY[vx];
			a2 = (a2 * param_tab[2]) >> 20;

			a0  = ccClip31[(a3 + a0 + dithr[vx&1])>>3];
			a1  = ccClip63[(a3 - a1 + dithg[vx&1])>>2];
			a2  = ccClip31[(a3 + a2 + dithb[vx&1])>>3];

			a0 = ((((a0<<6)|a1 )<<5))|a2;

			*((short*)outbuf) = (unsigned short)a0;
			outbuf += step_y;
		}
		start_outbuf += step_x;
	}	
}

void YUV411_to_RGB565_cc_mb_16x16_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
									VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
									const VO_S32 uin_stride, const VO_S32 vin_stride, VO_S32* const param_tab)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5,a7;
		//0 1
		LOAD_DATA(a3, a7, 0, 0)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 1)
		PIX_1(a3)
		WRITE_PIX(a4, 0)

		LOAD_YPIX(a3, 2)
		PIX_0(a3)
		LOAD_YPIX(a3, 3)
		PIX_1(a3)
		WRITE_PIX(a4, 4)

		LOAD_UPIX(a0, uin_stride)
		LOAD_VPIX(a1, vin_stride)
		CAL_UV()
		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 1)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		LOAD_YPIX(a7, in_stride + 2)
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 3)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 4)

		//2 3	
		LOAD_DATA(a3, a7, 1, 4)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 5)
		PIX_1(a3)
		WRITE_PIX(a4, 8)

		LOAD_YPIX(a3, 6)
		PIX_0(a3)
		LOAD_YPIX(a3, 7)
		PIX_1(a3)
		WRITE_PIX(a4, 12)

		LOAD_UPIX(a0, uin_stride + 1)
		LOAD_VPIX(a1, vin_stride + 1)
		CAL_UV()
		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 5)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		LOAD_YPIX(a7, in_stride + 6)
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 7)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 12)

		//4 5
		LOAD_DATA(a3, a7, 2, 8)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 9)
		PIX_1(a3)
		WRITE_PIX(a4, 16)

		LOAD_YPIX(a3, 10)
		PIX_0(a3)
		LOAD_YPIX(a3, 11)
		PIX_1(a3)
		WRITE_PIX(a4, 20)

		LOAD_UPIX(a0, uin_stride + 2)
		LOAD_VPIX(a1, vin_stride + 2)
		CAL_UV()
		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 9)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 16)

		LOAD_YPIX(a7, in_stride + 10)
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 11)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6 7
		LOAD_DATA(a3, a7, 3, 12)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 13)
		PIX_1(a3)
		WRITE_PIX(a4, 24)

		LOAD_YPIX(a3, 14)
		PIX_0(a3)
		LOAD_YPIX(a3, 15)
		PIX_1(a3)
		WRITE_PIX(a4, 28)

		LOAD_UPIX(a0, uin_stride + 3)
		LOAD_VPIX(a1, vin_stride + 3)
		CAL_UV()
		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 13)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 24)

		LOAD_YPIX(a7, in_stride + 14)
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride + 15)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 28)


		psrc_y  += (in_stride << 1);
		psrc_u  += (uin_stride << 1);
		psrc_v  += (vin_stride << 1);
		out_buf += (out_stride << 1);

	}while(--a6 != 0);

}

void YUV411_to_RGB565_cc_mb(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride, 
							VO_U8 *dst, const VO_S32 dst_stride, VO_S32 width, VO_S32 height,
							const VO_S32 uin_stride, const VO_S32 vin_stride, VO_S32* const param_tab)
{
	VO_U32 vx = 0, vy;
	const VO_U32 in_stride = src_stride , out_width = width, out_height = height;
	//const VO_S32 *param = yuv2rgbmatrix;
	VO_U8 *outbuf, *start_outbuf;
	VO_U8 *psrcY, *psrcU, *psrcV;
	VO_S32 *dithr, *dithg, *dithb;

	VO_S32 step_y, step_x, out_stride = dst_stride;

	/*start of output address*/
	step_y = 2;
	step_x = out_stride;
	start_outbuf = dst;

	for(vy = 0; vy < out_height; vy++) {
		outbuf = start_outbuf;
		vx =  vy * in_stride;
		psrcY = src_y + vx;
		vx =  vy * (in_stride >> 2);
		psrcU = src_u + vx;
		psrcV = src_v + vx;

		dithr = ditherrb[vy&1];
		dithb = ditherrb[(vy+1)&1];
		dithg = ditherg[vy&1];

		for(vx = 0; vx < out_width; vx++) {
			int a0, a1, a2, a3;

			a3 = psrcV[vx>> 2] - 128;
			a2 = psrcU[vx>> 2] - 128;

			a0 = (a3 * param_tab[0]) >> 20;
			a1 = (a3 * param_tab[1] + a2 *param_tab[3]) >> 20;
			a3 = psrcY[vx];
			a2 = (a2 * param_tab[2]) >> 20;

			a0  = ccClip31[(a3 + a0 + dithr[vx&1])>>3];
			a1  = ccClip63[(a3 - a1 + dithg[vx&1])>>2];
			a2  = ccClip31[(a3 + a2 + dithb[vx&1])>>3];

			a0 = ((((a0<<6)|a1 )<<5))|a2;

			*((short*)outbuf) = (unsigned short)a0;
			outbuf += step_y;
		}
		start_outbuf += step_x;
	}	
}

void YUV411V_to_RGB565_cc_mb_16x16_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
									 VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
									 const VO_S32 uin_stride, const VO_S32 vin_stride, VO_S32* const param_tab)
{
	VO_U32 a6 = 4;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5,a7;
		VO_S32 a0_1,a1_1,a2_1;

		//0
		LOAD_DATA(a3, a7, 0, 0)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 1)
		LOAD_UPIX(a0_1, 1)
		LOAD_VPIX(a1_1, 1)
		CAL_UVNEW()
		PIX_1NEW(a3)
		WRITE_PIX(a4, 0)

		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 1)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride)

		LOAD_YPIX(a3, in_stride*2)
		PIX_0(a3)
		LOAD_YPIX(a3, in_stride*2 + 1)
		PIX_1NEW(a3)
		WRITE_PIX(a4, out_stride*2)

		LOAD_YPIX(a7, in_stride*3)
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride*3 + 1)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride*3)

		//1	
		LOAD_DATA(a3, a7, 2, 2)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 3)
		LOAD_UPIX(a0_1, 3)
		LOAD_VPIX(a1_1, 3)
		CAL_UVNEW()
		PIX_1NEW(a3)
		WRITE_PIX(a4, 4)

		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 3)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride + 4)

		LOAD_YPIX(a3, in_stride*2 + 2)
		PIX_0(a3)
		LOAD_YPIX(a3, in_stride*2 + 3)
		PIX_1NEW(a3)
		WRITE_PIX(a4, out_stride*2 + 4)

		LOAD_YPIX(a7, in_stride*3 + 2)
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride*3 + 3)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride*3 + 4)

		//2
		LOAD_DATA(a3, a7, 4, 4)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 5)
		LOAD_UPIX(a0_1, 5)
		LOAD_VPIX(a1_1, 5)
		CAL_UVNEW()
		PIX_1NEW(a3)
		WRITE_PIX(a4, 8)

		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 5)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride + 8)

		LOAD_YPIX(a3, in_stride*2 + 4)
		PIX_0(a3)
		LOAD_YPIX(a3, in_stride*2 + 5)
		PIX_1NEW(a3)
		WRITE_PIX(a4, out_stride*2 + 8)

		LOAD_YPIX(a7, in_stride*3 + 4)
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride*3 + 5)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride*3 + 8)

		//3
		LOAD_DATA(a3, a7, 6, 6)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 7)
		LOAD_UPIX(a0_1, 7)
		LOAD_VPIX(a1_1, 7)
		CAL_UVNEW()
		PIX_1NEW(a3)
		WRITE_PIX(a4, 12)

		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 7)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride + 12)

		LOAD_YPIX(a3, in_stride*2 + 6)
		PIX_0(a3)
		LOAD_YPIX(a3, in_stride*2 + 7)
		PIX_1NEW(a3)
		WRITE_PIX(a4, out_stride*2 + 12)

		LOAD_YPIX(a7, in_stride*3 + 6)
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride*3 + 7)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride*3 + 12)

		//4
		LOAD_DATA(a3, a7, 8, 8)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 9)
		LOAD_UPIX(a0_1, 9)
		LOAD_VPIX(a1_1, 9)
		CAL_UVNEW()
		PIX_1NEW(a3)
		WRITE_PIX(a4, 16)

		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 9)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride + 16)

		LOAD_YPIX(a3, in_stride*2 + 8)
		PIX_0(a3)
		LOAD_YPIX(a3, in_stride*2 + 9)
		PIX_1NEW(a3)
		WRITE_PIX(a4, out_stride*2 + 16)

		LOAD_YPIX(a7, in_stride*3 + 8)
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride*3 + 9)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride*3 + 16)

		//5
		LOAD_DATA(a3, a7, 10, 10)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 11)
		LOAD_UPIX(a0_1, 11)
		LOAD_VPIX(a1_1, 11)
		CAL_UVNEW()
		PIX_1NEW(a3)
		WRITE_PIX(a4, 20)

		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 11)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride + 20)

		LOAD_YPIX(a3, in_stride*2 + 10)
		PIX_0(a3)
		LOAD_YPIX(a3, in_stride*2 + 11)
		PIX_1NEW(a3)
		WRITE_PIX(a4, out_stride*2 + 20)

		LOAD_YPIX(a7, in_stride*3 + 10)
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride*3 + 11)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride*3 + 20)

		//6
		LOAD_DATA(a3, a7, 12, 12)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 13)
		LOAD_UPIX(a0_1, 13)
		LOAD_VPIX(a1_1, 13)
		CAL_UVNEW()
		PIX_1NEW(a3)
		WRITE_PIX(a4, 24)

		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 13)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride + 24)

		LOAD_YPIX(a3, in_stride*2 + 12)
		PIX_0(a3)
		LOAD_YPIX(a3, in_stride*2 + 13)
		PIX_1NEW(a3)
		WRITE_PIX(a4, out_stride*2 + 24)

		LOAD_YPIX(a7, in_stride*3 + 12)
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride*3 + 13)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride*3 + 24)

		//7
		LOAD_DATA(a3, a7, 14, 14)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 15)
		LOAD_UPIX(a0_1, 15)
		LOAD_VPIX(a1_1, 15)
		CAL_UVNEW()
		PIX_1NEW(a3)
		WRITE_PIX(a4, 28)

		PIX_2(a7)			
		LOAD_YPIX(a7, in_stride + 15)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride + 28)

		LOAD_YPIX(a3, in_stride*2 + 14)
		PIX_0(a3)
		LOAD_YPIX(a3, in_stride*2 + 15)
		PIX_1NEW(a3)
		WRITE_PIX(a4, out_stride*2 + 28)

		LOAD_YPIX(a7, in_stride*3 + 14)
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride*3 + 15)
		PIX_3NEW(a7)
		WRITE_PIX(a4, out_stride*3 + 28)

		psrc_y += (in_stride<<2);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf += (out_stride<<2);

	}while(--a6 != 0);

}

void YUV411V_to_RGB565_cc_mb(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride, 
							 VO_U8 *dst, const VO_S32 dst_stride, VO_S32 width, VO_S32 height,
							 const VO_S32 uin_stride, const VO_S32 vin_stride, VO_S32* const param_tab)
{
	VO_U32 vx = 0, vy;
	const VO_U32 in_stride = src_stride , out_width = width, out_height = height;
	//const VO_S32 *param = yuv2rgbmatrix;
	VO_U8 *outbuf, *start_outbuf;
	VO_U8 *psrcY, *psrcU, *psrcV;
	VO_S32 *dithr, *dithg, *dithb;

	VO_S32 step_y, step_x, out_stride = dst_stride;

	/*start of output address*/
	step_y = 2;
	step_x = out_stride;
	start_outbuf = dst;

	for(vy = 0; vy < out_height; vy++) {
		outbuf = start_outbuf;
		vx =  vy * in_stride;
		psrcY = src_y + vx;
		vx =  (vy>>2)*in_stride;
		psrcU = src_u + vx;
		psrcV = src_v + vx;

		dithr = ditherrb[vy&1];
		dithb = ditherrb[(vy+1)&1];
		dithg = ditherg[vy&1];

		for(vx = 0; vx < out_width; vx++) {
			int a0, a1, a2, a3;

			a3 = psrcV[vx] - 128;
			a2 = psrcU[vx] - 128;

			a0 = (a3 * param_tab[0]) >> 20;
			a1 = (a3 * param_tab[1] + a2 *param_tab[3]) >> 20;
			a3 = psrcY[vx];
			a2 = (a2 * param_tab[2]) >> 20;

			a0  = ccClip31[(a3 + a0 + dithr[vx&1])>>3];
			a1  = ccClip63[(a3 - a1 + dithg[vx&1])>>2];
			a2  = ccClip31[(a3 + a2 + dithb[vx&1])>>3];

			a0 = ((((a0<<6)|a1 )<<5))|a2;

			*((short*)outbuf) = (unsigned short)a0;
			outbuf += step_y;
		}
		start_outbuf += step_x;
	}	
}
#endif //COLCONVENABLE

#define GET_U(a0, a3){\
	GET_PIX1(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX3(a1, a3)\
	a1 -= 128;\
}

#ifdef COLCONVENABLE
void YUYVToRGB16_MB_rotation_no(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf += (out_stride<<1);

	}while(--a6 != 0);
}

void YUYV2ToRGB16_MB_rotation_no(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf += (out_stride<<1);

	}while(--a6 != 0);
}
#endif //COLCONVENABLE

#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX3(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX1(a1, a3)\
	a1 -= 128;\
}

#ifdef COLCONVENABLE
void YVYUToRGB16_MB_rotation_no(VO_U8 *psrc, const VO_S32 in_stride,VO_U8 *out_buf, const VO_S32 out_stride,
								const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf += (out_stride<<1);

	}while(--a6 != 0);
}

void YVYU2ToRGB16_MB_rotation_no(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf += (out_stride<<1);

	}while(--a6 != 0);
}
#endif //COLCONVENABLE

#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX0(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX2(a1, a3)\
	a1 -= 128;\
}

#ifdef COLCONVENABLE
void UYVYToRGB16_MB_rotation_no(VO_U8 *psrc, const VO_S32 in_stride,VO_U8 *out_buf, const VO_S32 out_stride, 
								const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf += (out_stride<<1);

	}while(--a6 != 0);
}

void UYVY2ToRGB16_MB_rotation_no(VO_U8 *psrc, const VO_S32 in_stride,VO_U8 *out_buf, const VO_S32 out_stride,
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf += (out_stride<<1);

	}while(--a6 != 0);
}
#endif //COLCONVENABLE

#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX2(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX0(a1, a3)\
	a1 -= 128;\
}

#ifdef COLCONVENABLE 
void VYUYToRGB16_MB_rotation_no(VO_U8 *psrc, const VO_S32 in_stride,VO_U8 *out_buf, const VO_S32 out_stride,
								const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf += (out_stride<<1);

	}while(--a6 != 0);
}

void VYUY2ToRGB16_MB_rotation_no(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf += (out_stride<<1);

	}while(--a6 != 0);
}
#endif //COLCONVENABLE



void cc_yuv420_rgb32_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];

			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a2 -= 128;
			a3 -= 128;

			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip255[(((a4 + a0)>>20) + RDITHERNEW_P0)])<<16)|((ccClip255[(((a4 - a1)>>20) + GDITHERNEW_P0)])<<8)|(ccClip255[(((a4 + a2)>>20) + BDITHERNEW_P0)]) | ( alpha_value << 24 );
			a7 = ((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P1)])<<16)|((ccClip255[(((a5 - a1)>>20) + GDITHERNEW_P1)])<<8)|(ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P1)]) | ( alpha_value << 24 );

			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P2)])<<16)|((ccClip255[(((a5 - a1)>>20) + GDITHERNEW_P2)])<<8)|(ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P2)]) | ( alpha_value << 24 );//|a4;
			a8 = ((ccClip255[(((a8 + a0)>>20) + RDITHERNEW_P3)])<<16)|((ccClip255[(((a8 - a1)>>20) + GDITHERNEW_P3)])<<8)|(ccClip255[(((a8 + a2)>>20) + BDITHERNEW_P3)]) | ( alpha_value << 24 );//|a7;

			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst + 4)) = a5;
			*((VO_U32*)(dst+dst_stride)) = a7;
			*((VO_U32*)(dst+dst_stride+4)) = a8;
			dst += 8;
		}while((i-=2) != 0);

		dst -= (width<<2);
		dst += (dst_stride<<1);
		y_scale_par += 6;
	}while((height-=2) != 0);

}

void cc_yuv420_rgb32_mb_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
								 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			a4 = ((ccClip255[(a4 + a0)>>20])<<16)|((ccClip255[(a4 - a1)>>20])<<8)|(ccClip255[(a4 + a2)>>20]) | ( alpha_value << 24 );
			a7 = ((ccClip255[(a5 + a0)>>20])<<16)|((ccClip255[(a5 - a1)>>20])<<8)|(ccClip255[(a5 + a2)>>20]) | ( alpha_value << 24 );

			a5 = src_y0[a6];
			a8 = src_y1[a6];

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip255[(a5 + a0)>>20])<<16)|((ccClip255[(a5 - a1)>>20])<<8)|(ccClip255[(a5 + a2)>>20]) | ( alpha_value << 24 );//|a4;
			a8 = ((ccClip255[(a8 + a0)>>20])<<16)|((ccClip255[(a8 - a1)>>20])<<8)|(ccClip255[(a8 + a2)>>20]) | ( alpha_value << 24 );//|a7;

			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst + 4)) = a5;
			*((VO_U32*)(dst+dst_stride)) = a7;
			*((VO_U32*)(dst+dst_stride+4)) = a8;
			x_scale_par0 += 6;
			dst += 8;
		}while((i-=2) != 0);

		dst -= (width<<2);
		dst += (dst_stride<<1);
		y_scale_par += 6;
	}while((height-=2) != 0);

}

void cc_yuv420_argb32_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];

			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a2 -= 128;
			a3 -= 128;

			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip255[(((a4 + a0)>>20) + RDITHERNEW_P0)]))|((ccClip255[(((a4 - a1)>>20) + GDITHERNEW_P0)])<<8)|((ccClip255[(((a4 + a2)>>20) + BDITHERNEW_P0)])<<16)|(255<<24);
			a7 = ((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P1)]))|((ccClip255[(((a5 - a1)>>20) + GDITHERNEW_P1)])<<8)|((ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P1)])<<16)|(255<<24);

			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P2)]))|((ccClip255[(((a5 - a1)>>20) + GDITHERNEW_P2)])<<8)|((ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P2)])<<16)|(255<<24);//|a4;
			a8 = ((ccClip255[(((a8 + a0)>>20) + RDITHERNEW_P3)]))|((ccClip255[(((a8 - a1)>>20) + GDITHERNEW_P3)])<<8)|((ccClip255[(((a8 + a2)>>20) + BDITHERNEW_P3)])<<16)|(255<<24);//|a7;

			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst + 4)) = a5;
			*((VO_U32*)(dst+dst_stride)) = a7;
			*((VO_U32*)(dst+dst_stride+4)) = a8;
			dst += 8;
		}while((i-=2) != 0);

		dst -= (width<<2);
		dst += (dst_stride<<1);
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_yuv420_argb32_mb_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							      VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			a0 = x_scale_par0[0];//src pos0

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];

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

			a4 = ((ccClip255[(a4 + a0)>>20]))|((ccClip255[(a4 - a1)>>20])<<8)|((ccClip255[(a4 + a2)>>20])<<16)|(255<<24);
			a7 = ((ccClip255[(a5 + a0)>>20]))|((ccClip255[(a5 - a1)>>20])<<8)|((ccClip255[(a5 + a2)>>20])<<16)|(255<<24);

			x_scale_par0 += 3;

			a5 = src_y0[a6];
			a8 = src_y1[a6];

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip255[(a5 + a0)>>20]))|((ccClip255[(a5 - a1)>>20])<<8)|((ccClip255[(a5 + a2)>>20])<<16)|(255<<24);//|a4;
			a8 = ((ccClip255[(a8 + a0)>>20]))|((ccClip255[(a8 - a1)>>20])<<8)|((ccClip255[(a8 + a2)>>20])<<16)|(255<<24);//|a7;

			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst + 4)) = a5;
			*((VO_U32*)(dst+dst_stride)) = a7;
			*((VO_U32*)(dst+dst_stride+4)) = a8;
			dst += 8;
		}while((i-=2) != 0);

		dst -= (width<<2);
		dst += (dst_stride<<1);
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_yuv420_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
					  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a2 -= 128;
			a3 -= 128;

			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////
			//			a6 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

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

void cc_yuv420_mb_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
						   VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////
			a5 = src_y0[a6];
			a8 = src_y1[a6];

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

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


//3quarter
void cc_yuv420_mb_s_c_3quarter(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							   VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;

	do{
		//y0
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);		
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		VO_S32 first1 = 0x155;//y_scale_par[4];
		VO_S32 first2 = 0x2ab;//y_scale_par[5];
		VO_S32 second1 = 0x2aa;//x_scale_par0[1];
		VO_S32 second2 = 0x156;//x_scale_par0[2];	

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//			x_scale_par0 += 3;
			a5 = (src_y0[a6]*first2 + src_y0[a6+1]*first1)>>10;
			a8 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*first2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*first1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

			//x1
			a0 = x_scale_par0[6];
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = src_y0[a6];
			a8 = (src_y1[a6]*first2 + src_y1[a6+src_stride]*first1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = (src_y0[a6]*second2 + src_y0[a6+1]*second1)>>10;
			a8 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*first2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*first1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;
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
		src_y1 = src_y + y_scale_par[3] * src_stride;

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;

			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////
			//			aa = 0x155;////x_scale_par0[1];
			//			bb = 0x2ab;//x_scale_par0[2];

			//	x_scale_par0 += 3;
			a5 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*second2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*second1)>>20;
			a8 = (src_y1[a6]*first2 + src_y1[a6+1]*first1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;


			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = (src_y0[a6]*second2 + src_y0[a6+src_stride]*second1)>>10;
			a8 = src_y1[a6];

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*second2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*second1)>>20;
			a8 = (src_y1[a6]*second2 + src_y1[a6+1]*second1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;
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
		src_y1 = src_y + y_scale_par[3] * src_stride;

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*first2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*first1)>>20;
			a8 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*second2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*second1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;


			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = (src_y0[a6]*first2 + src_y0[a6+src_stride]*first1)>>10;
			a8 = (src_y1[a6]*second2 + src_y1[a6+src_stride]*second1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*first2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*first1)>>20;
			a8 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*second2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*second1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;
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

//1point5
void cc_yuv420_mb_s_c_1point5(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v,VO_U8 *dst, VO_S32 src_stride,
							  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;

	do{
		//y0
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		VO_S32 second1 = 0x155;//y_scale_par[4];
		VO_S32 second2= 0x2ab;//y_scale_par[5];
		VO_S32 first1= 0x2aa;//x_scale_par0[1];
		VO_S32 first2 = 0x156;//x_scale_par0[2];	

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = (src_y0[a6]*first2 + src_y0[a6+1]*first1)>>10;
			a8 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*first2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*first1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

			//x1
			a0 = x_scale_par0[6];
			//aa = 0x2aa;//x_scale_par0[1];
			//bb = 0x156;//x_scale_par0[2];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = src_y0[a6];
			a8 = (src_y1[a6]*first2 + src_y1[a6+src_stride]*first1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = (src_y0[a6]*second2 + src_y0[a6+1]*second1)>>10;
			a8 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*first2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*first1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;
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

		//aa0 = 0x2aa;//y_scale_par[1];
		//bb0 = 0x156;//y_scale_par[2];

		src_y1 = src_y + y_scale_par[3] * src_stride;

		//aa = 0x155;////x_scale_par0[1];
		//bb = 0x2ab;//x_scale_par0[2];

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;

			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////
			//aa = 0x155;////x_scale_par0[1];
			//bb = 0x2ab;//x_scale_par0[2];

			//x_scale_par0 += 3;
			a5 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*second2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*second1)>>20;
			a8 = (src_y1[a6]*first2 + src_y1[a6+1]*first1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;


			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = (src_y0[a6]*second2 + src_y0[a6+src_stride]*second1)>>10;
			a8 = src_y1[a6];

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*second2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*second1)>>20;
			a8 = (src_y1[a6]*second2 + src_y1[a6+1]*second1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;
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

		//aa0 = 0x155;//y_scale_par[1];
		//bb0 = 0x2ab;//y_scale_par[2];
		src_y1 = src_y + y_scale_par[3] * src_stride;

		//aa1 = 0x2aa;//y_scale_par[4];
		//bb1 = 0x156;//y_scale_par[5];	

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*first2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*first1)>>20;
			a8 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*second2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*second1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;


			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = (src_y0[a6]*first2 + src_y0[a6+src_stride]*first1)>>10;
			a8 = (src_y1[a6]*second2 + src_y1[a6+src_stride]*second1)>>10;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

			a4 |= (a5 << 16);
			a7 |= (a8 << 16);
			*((VO_U32*)dst) = a4;
			*((VO_U32*)(dst+dst_stride)) = a7;
			dst += 4;

			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a5 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*first2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*first1)>>20;
			a8 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*second2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*second1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;
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

//double
void cc_yuv420_mb_s_c_double(VO_S32 width, VO_S32 height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////

			a5 = (src_y0[a6] + src_y0[a6+1])>>1;
			a8 = (src_y1[a6] + src_y1[a6+1] + src_y1[a6+src_stride] + src_y1[a6+src_stride+1])>>2;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

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

#ifdef COLCONVENABLE

void yuv444_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							 VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;

	do{
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + y_scale_par[0] * uin_stride;
		VO_U8 *src_v0 = src_v + y_scale_par[0] * vin_stride;

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;

		VO_S32 aa1 = y_scale_par[4];
		VO_S32 bb1 = y_scale_par[5];	

		i = width;		
		do{
			VO_S32 a0, a4, a5, a6, a7, a8;
			VO_S32 aa, bb;
			VO_S32 a00,a01,a02,a03;
			VO_S32 a10,a11,a12,a13;
			VO_S32 a20,a21,a22,a23;
			VO_S32 a30,a31,a32,a33;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			//			a1 = ((a0+1)>>1);
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];

			a20 = src_u0[a0];
			a30 = src_v0[a0];

			a22 = src_u0[a0 + uin_stride];
			a32 = src_v0[a0 + vin_stride];

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a20 -= 128;
			a30 -= 128;
			a22 -= 128;
			a32 -= 128;

			a00 = (a30 * ConstV1);
			a10 = (a30 * ConstV2 + a20 *ConstU2);
			a20 = (a20 * ConstU1);

			a02 = (a32 * ConstV1);
			a12 = (a32 * ConstV2 + a22 *ConstU2);
			a22 = (a22 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a00)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a10)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a20)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a7 = ((ccClip63[(((a5 + a02)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a12)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a22)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////
			//			a6 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;

			a21 = src_u0[a6];
			a31 = src_v0[a6];
			a23 = src_u0[a6 + uin_stride];
			a33 = src_v0[a6 + vin_stride];

			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a21 -= 128;
			a31 -= 128;
			a23 -= 128;
			a33 -= 128;

			a01 = (a31 * ConstV1);
			a11 = (a31 * ConstV2 + a21 *ConstU2);
			a21 = (a21 * ConstU1);

			a03 = (a33 * ConstV1);
			a13 = (a33 * ConstV2 + a23 *ConstU2);
			a23 = (a23 * ConstU1);

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a01)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a11)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a21)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a03)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a13)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a23)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

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

void yuv422_21_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
								VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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
			VO_S32 a0, a4, a5, a6, a7, a8;
			VO_S32 aa, bb;
			VO_S32 a00,a01;
			VO_S32 a10,a11;
			VO_S32 a20,a21;
			VO_S32 a30,a31;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			//			a1 = ((a0+1)>>1);
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];

			a20 = src_u0[a0];
			a30 = src_v0[a0];

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a20 -= 128;
			a30 -= 128;

			a00 = (a30 * ConstV1);
			a10 = (a30 * ConstV2 + a20 *ConstU2);
			a20 = (a20 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a00)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a10)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a20)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a7 = ((ccClip63[(((a5 + a00)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a10)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a20)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////
			//a6 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;

			a21 = src_u0[a6];
			a31 = src_v0[a6];

			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a21 -= 128;
			a31 -= 128;

			a01 = (a31 * ConstV1);
			a11 = (a31 * ConstV2 + a21 *ConstU2);
			a21 = (a21 * ConstU1);

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a01)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a11)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a21)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a01)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a11)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a21)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

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

void yuv411_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							 VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;

	do{
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + y_scale_par[0] * uin_stride;
		VO_U8 *src_v0 = src_v + y_scale_par[0] * vin_stride;

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		VO_U8 *src_u1 = src_u + y_scale_par[3] * uin_stride;
		VO_U8 *src_v1 = src_v + y_scale_par[3] * vin_stride;

		VO_S32 aa1 = y_scale_par[4];
		VO_S32 bb1 = y_scale_par[5];	

		i = width;		
		do{
			VO_S32 a0, a1, a4, a5, a6, a7, a8;
			VO_S32 a00,a01;
			VO_S32 a10,a11;
			VO_S32 a20,a21;
			VO_S32 a30,a31;
			VO_S32 x_par0, x_par1;
			VO_S32 y_par0, y_par1;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			x_par0 = x_scale_par0[1];
			y_par0 = x_scale_par0[2];

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];
			x_par1 = x_scale_par0[1];
			y_par1 = x_scale_par0[2];

			a1 = (a0 + a6 + 4)>>3;

			a20 = src_u0[a1];
			a30 = src_v0[a1];
			a21 = src_u1[a1];
			a31 = src_v1[a1];

			a4 = ((src_y0[a0]*y_par0 + src_y0[a0+1]*x_par0)*bb0 + (src_y0[a0+src_stride]*y_par0 + src_y0[a0+src_stride+1]*x_par0)*aa0)>>20;
			a5 = ((src_y1[a0]*y_par0 + src_y1[a0+1]*x_par0)*bb1 + (src_y1[a0+src_stride]*y_par0 + src_y1[a0+src_stride+1]*x_par0)*aa1)>>20;

			a20 -= 128;
			a30 -= 128;

			a00 = (a30 * ConstV1);
			a10 = (a30 * ConstV2 + a20 *ConstU2);
			a20 = (a20 * ConstU1);

			a21 -= 128;
			a31 -= 128;

			a01 = (a31 * ConstV1);
			a11 = (a31 * ConstV2 + a21 *ConstU2);
			a21 = (a21 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a00)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a10)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a20)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a7 = ((ccClip63[(((a5 + a01)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a11)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a21)>>20) + BDITHERNEW_P1)>>2]>>1);

			a5 = ((src_y0[a6]*y_par1 + src_y0[a6+1]*x_par1)*bb0 + (src_y0[a6+src_stride]*y_par1 + src_y0[a6+src_stride+1]*x_par1)*aa0)>>20;
			a8 = ((src_y1[a6]*y_par1 + src_y1[a6+1]*x_par1)*bb1 + (src_y1[a6+src_stride]*y_par1 + src_y1[a6+src_stride+1]*x_par1)*aa1)>>20;


			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a00)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a10)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a20)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a01)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a11)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a21)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

			x_scale_par0 += 3;

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

void yuv411V_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							  VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;

	do{
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + (y_scale_par[0] >> 2) * uin_stride;
		VO_U8 *src_v0 = src_v + (y_scale_par[0] >> 2) * vin_stride;

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];

		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		VO_U8 *src_u1 = src_u + (y_scale_par[3] >> 2) * uin_stride;
		VO_U8 *src_v1 = src_v + (y_scale_par[3] >> 2) * vin_stride;
		VO_S32 aa1 = y_scale_par[4];
		VO_S32 bb1 = y_scale_par[5];	

		i = width;		
		do{
			VO_S32 a0, a4, a5, a6, a7, a8;
			VO_S32 aa, bb;
			VO_S32 a00,a01;
			VO_S32 a10,a11;
			VO_S32 a20,a21;
			VO_S32 a30,a31;

			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];

			a20 = src_u0[a0];
			a30 = src_v0[a0];

			a21 = src_u1[a0];
			a31 = src_v1[a0];

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a20 -= 128;
			a30 -= 128;
			a21 -= 128;
			a31 -= 128;

			a00 = (a30 * ConstV1);
			a10 = (a30 * ConstV2 + a20 *ConstU2);
			a20 = (a20 * ConstU1);

			a01 = (a31 * ConstV1);
			a11 = (a31 * ConstV2 + a21 *ConstU2);
			a21 = (a21 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a00)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a10)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a20)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a7 = ((ccClip63[(((a5 + a01)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a11)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a21)>>20) + BDITHERNEW_P1)>>2]>>1);


			/////////////////////////////////////////////////////////////////////
			//a6 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;

			a20 = src_u0[a6];
			a30 = src_v0[a6];

			a21 = src_u1[a6];
			a31 = src_v1[a6];

			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a20 -= 128;
			a30 -= 128;
			a21 -= 128;
			a31 -= 128;

			a00 = (a30 * ConstV1);
			a10 = (a30 * ConstV2 + a20 *ConstU2);
			a20 = (a20 * ConstU1);

			a01 = (a31 * ConstV1);
			a11 = (a31 * ConstV2 + a21 *ConstU2);
			a21 = (a21 * ConstU1);

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a00)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a10)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a5 + a20)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a01)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a11)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a8 + a21)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

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
#endif //COLCONVENABLE

void cc_mb_16x16_l90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
					   VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
					   const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7;
		
		//0
		NEWLOAD_DATA(a3, a7, 0, 0)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 1)
		NEWPIX_1(a7)

		WRITE90L_PIX(a4, 0)
		NEWPIX_2(a3)
		NEWLOAD_YPIX(a7, in_stride+1)
		NEWPIX_3(a7)

		WRITE90L_PIX(a4, -out_stride)

		//1
		NEWLOAD_DATA(a3, a7, 1, 2)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 3)
		NEWPIX_1(a7)

		WRITE90L_PIX(a4, -out_stride)
		NEWPIX_2(a3)
		NEWLOAD_YPIX(a7, in_stride+3)
		NEWPIX_3(a7)

		WRITE90L_PIX(a4, -out_stride)

		//2
		NEWLOAD_DATA(a3, a7, 2, 4)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 5)
		NEWPIX_1(a7)

		WRITE90L_PIX(a4, -out_stride)
		NEWPIX_2(a3)
		NEWLOAD_YPIX(a7, in_stride+5)
		NEWPIX_3(a7)

		WRITE90L_PIX(a4, -out_stride)

		//3
		NEWLOAD_DATA(a3, a7, 3, 6)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 7)
		NEWPIX_1(a7)

		WRITE90L_PIX(a4, -out_stride)
		NEWPIX_2(a3)
		NEWLOAD_YPIX(a7, in_stride+7)
		NEWPIX_3(a7)

		WRITE90L_PIX(a4, -out_stride)

		//4
		NEWLOAD_DATA(a3, a7, 4, 8)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 9)
		NEWPIX_1(a7)

		WRITE90L_PIX(a4, -out_stride)
		NEWPIX_2(a3)
		NEWLOAD_YPIX(a7, in_stride+9)
		NEWPIX_3(a7)

		WRITE90L_PIX(a4, -out_stride)

		//5
		NEWLOAD_DATA(a3, a7, 5, 10)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 11)
		NEWPIX_1(a7)

		WRITE90L_PIX(a4, -out_stride)
		NEWPIX_2(a3)
		NEWLOAD_YPIX(a7, in_stride+11)
		NEWPIX_3(a7)

		WRITE90L_PIX(a4, -out_stride)

		//6
		NEWLOAD_DATA(a3, a7, 6, 12)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 13)
		NEWPIX_1(a7)

		WRITE90L_PIX(a4, -out_stride)
		NEWPIX_2(a3)
		NEWLOAD_YPIX(a7, in_stride+13)
		NEWPIX_3(a7)

		WRITE90L_PIX(a4, -out_stride)

		//7
		NEWLOAD_DATA(a3, a7, 7, 14)
		NEWCAL_UV()
		NEWPIX_0(a3)
		NEWLOAD_YPIX(a3, 15)
		NEWPIX_1(a7)

		WRITE90L_PIX(a4, -out_stride)
		NEWPIX_2(a3)
		NEWLOAD_YPIX(a7, in_stride+15)
		NEWPIX_3(a7)

		WRITE90L_PIX(a4, -out_stride)

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf+=-out_stride;
		out_buf+=4;					
		a7 = out_stride<<4;
		out_buf += a7;

	}while(--a6 != 0);
}

void cc_mb_16x16_r90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
					   VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
					   const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7;

		//0
		NEWLOAD_DATA(a3, a7, 0, 0)
		NEWCAL_UV()
		NEWPIX90R_0(a3)
		NEWLOAD_YPIX(a3, 1)
		NEWPIX90R_1(a7)

		WRITE90R_PIX(a4, 0)
		NEWPIX90R_2(a3)
		NEWLOAD_YPIX(a7, in_stride+1)
		NEWPIX90R_3(a7)

		WRITE90R_PIX(a4, out_stride)

		//1
		NEWLOAD_DATA(a3, a7, 1, 2)
		NEWCAL_UV()
		NEWPIX90R_0(a3)
		NEWLOAD_YPIX(a3, 3)
		NEWPIX90R_1(a7)

		WRITE90R_PIX(a4, out_stride)
		NEWPIX90R_2(a3)
		NEWLOAD_YPIX(a7, in_stride+3)
		NEWPIX90R_3(a7)

		WRITE90R_PIX(a4, out_stride)

		//2
		NEWLOAD_DATA(a3, a7, 2, 4)
		NEWCAL_UV()
		NEWPIX90R_0(a3)
		NEWLOAD_YPIX(a3, 5)
		NEWPIX90R_1(a7)

		WRITE90R_PIX(a4, out_stride)
		NEWPIX90R_2(a3)
		NEWLOAD_YPIX(a7, in_stride+5)
		NEWPIX90R_3(a7)

		WRITE90R_PIX(a4, out_stride)

		//3
		NEWLOAD_DATA(a3, a7, 3, 6)
		NEWCAL_UV()
		NEWPIX90R_0(a3)
		NEWLOAD_YPIX(a3, 7)
		NEWPIX90R_1(a7)

		WRITE90R_PIX(a4, out_stride)
		NEWPIX90R_2(a3)
		NEWLOAD_YPIX(a7, in_stride+7)
		NEWPIX90R_3(a7)

		WRITE90R_PIX(a4, out_stride)

		//4
		NEWLOAD_DATA(a3, a7, 4, 8)
		NEWCAL_UV()
		NEWPIX90R_0(a3)
		NEWLOAD_YPIX(a3, 9)
		NEWPIX90R_1(a7)

		WRITE90R_PIX(a4, out_stride)
		NEWPIX90R_2(a3)
		NEWLOAD_YPIX(a7, in_stride+9)
		NEWPIX90R_3(a7)

		WRITE90R_PIX(a4, out_stride)

		//5
		NEWLOAD_DATA(a3, a7, 5, 10)
		NEWCAL_UV()
		NEWPIX90R_0(a3)
		NEWLOAD_YPIX(a3, 11)
		NEWPIX90R_1(a7)

		WRITE90R_PIX(a4, out_stride)
		NEWPIX90R_2(a3)
		NEWLOAD_YPIX(a7, in_stride+11)
		NEWPIX90R_3(a7)

		WRITE90R_PIX(a4, out_stride)

		//6
		NEWLOAD_DATA(a3, a7, 6, 12)
		NEWCAL_UV()
		NEWPIX90R_0(a3)
		NEWLOAD_YPIX(a3, 13)
		NEWPIX90R_1(a7)

		WRITE90R_PIX(a4, out_stride)
		NEWPIX90R_2(a3)
		NEWLOAD_YPIX(a7, in_stride+13)
		NEWPIX90R_3(a7)

		WRITE90R_PIX(a4, out_stride)

		//7
		NEWLOAD_DATA(a3, a7, 7, 14)
		NEWCAL_UV()
		NEWPIX90R_0(a3)
		NEWLOAD_YPIX(a3, 15)
		NEWPIX90R_1(a7)

		WRITE90R_PIX(a4, out_stride)
		NEWPIX90R_2(a3)
		NEWLOAD_YPIX(a7, in_stride+15)
		NEWPIX90R_3(a7)

		WRITE90R_PIX(a4, out_stride)

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf+=out_stride;
		out_buf-= 4;	
		a7 = out_stride<<4;
		out_buf -= a7;

	}while(--a6 != 0);

}
void cc_mb_16x16_180_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
					   VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
					   const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8,num;
	out_buf += (out_stride<<1) * 8 - out_stride;
	do{
		VO_S32 a0, a1, a2, a3, a5, a7;
		VO_U32 a4;

		for(num = 0; num<8; num++)
		{
			a0 = psrc_u[num] - 128;
			a1 = psrc_v[num] - 128;
			a3 = (psrc_y[2*num] - 16)*ConstY;
			a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;
			a2 = (a0 * ConstU1);
			a0 = (a1 * ConstV2 + a0 *ConstU2);
			a1 = (a1 * ConstV1);

			a4  = (ccClip63[((a3 + a1)>>22)])>>1;
			a5  = (ccClip63[((a3 - a0)>>22)]);
			a3  = (ccClip63[((a3 + a2)>>22)])>>1;
			a4 = (a4<<11)|(a5<<5)|a3;

			a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;
			a5  = (ccClip63[((a3 + a2)>>22)])>>1;
			a4 |= (a5 << 16);
			a5  = (ccClip63[((a3 + a1)>>22)])>>1;

			a3  = (ccClip63[((a3 - a0)>>22)]);
			a3 |= (a5<<6);
			a4 |= (a3 << 21);
			a4 = (a4>>16)|(a4<<16);
			*((VO_S32*)(out_buf+(4*(7 - num)))) = (VO_S32)a4;

			a4  = (ccClip63[((a7 + a1)>>22)])>>1;
			a5  = (ccClip63[((a7 - a0)>>22)]);
			a7  = (ccClip63[((a7 + a2)>>22)])>>1;
			a4 = (a4<<11)|(a5<<5)|a7;
			a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;
			a5  = (ccClip63[((a7 + a2)>>22)])>>1;
			a4 |= (a5 << 16);
			a5  = (ccClip63[((a7 + a1)>>22)])>>1;
			a7  = (ccClip63[((a7 - a0)>>22)]);
			a7 |= (a5<<6);
			a4 |= (a7 << 21);
			a4 = (a4>>16)|(a4<<16);
			*((VO_S32*)(out_buf+(4*(7 - num) - out_stride))) = (VO_S32)a4;
		} 
		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}

//huwei 20110530 bug fixed
VOCCRETURNCODE NewYUVPlanarToRGB16_normal(ClrConvData *conv_data, const VO_U32 isRGB565, const VO_U32 is420Planar,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	const VO_U32 in_stride = conv_data->nInStride , out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	const VO_U32 in_uvstride = conv_data->nInUVStride;
	//const VO_S32 *param = cc_hnd->yuv2rgbmatrix;
	VO_U8 *outbuf, *start_outbuf=NULL;
	VO_U8 *psrcY, *psrcU, *psrcV;
	VO_S32 step_y=0, step_x=0, out_stride = conv_data->nOutStride;

	/*start of output address*/
	if(conv_data->nRotationType == ROTATION_DISABLE){/*disable rotation*/
		step_y = 2;
		step_x = out_stride;
		start_outbuf = conv_data->pOutBuf[0];
	}else if (conv_data->nRotationType == ROTATION_180){/*rotation180*/
		step_y = -2;
		step_x = -out_stride;
		start_outbuf = conv_data->pOutBuf[0] - 2;
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
		psrcY = conv_data->pInBuf[0] + vx;
		vx = (vy>>is420Planar)*in_uvstride;
		psrcU = conv_data->pInBuf[1] + vx;
		psrcV = conv_data->pInBuf[2] + vx;

		for(vx = 0; vx < out_width; vx++) {
			int a0, a1, a2, a3;

			a3 = psrcV[vx>>1] - 128;
			a2 = psrcU[vx>>1] - 128;

			a0 = a3 * ConstV1;
			a1 = a3 * ConstV2 + a2 *ConstU2;
			a2 = a2 * ConstU1;
			a3 = (psrcY[vx] - 16)*ConstY;


			a0  = (ccClip63[(a3 + a0 )>>22]>>1);
			a1  = ccClip63[(a3 - a1 )>>22];
			a2  = (ccClip63[(a3 + a2 )>>22]>>1);

			a0 = ((((a0<<6)|a1 )<<5))|a2;

			*((short*)outbuf) = (unsigned short)a0;
			outbuf += step_y;
		}
		start_outbuf += step_x;
	}	
	return VO_ERR_NONE;
}

#ifdef COLCONVENABLE
void cc_rgb24_mb_16x16_l90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
							 VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
							 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 4;

	do{
		VO_S32 a0_temp0, a1_temp0, a2_temp0, a3_temp0, a4_temp0, a5_temp0, a7_temp0;
		VO_S32 a0_temp1, a1_temp1, a2_temp1, a3_temp1, a4_temp1, a5_temp1, a7_temp1;
		VO_S32 temp0, temp1, temp2;
		VO_S32 number;

		for(number = 0; number< 8; number++)
		{
			a0_temp0 = psrc_u[number] - 128;
			a1_temp0 = psrc_v[number] - 128;
			a3_temp0 = (psrc_y[2*number] - 16)*ConstY;
			a7_temp0 = (psrc_y[in_stride+2*number] - 16)*ConstY;

			a0_temp1 = psrc_u[number + uin_stride] - 128;
			a1_temp1 = psrc_v[number + vin_stride] - 128;
			a3_temp1 = (psrc_y[2*number + (in_stride << 1)] - 16)*ConstY;
			a7_temp1 = (psrc_y[2*number + (in_stride << 1) + in_stride] - 16)*ConstY;

			a2_temp0 = (a0_temp0 * ConstU1);
			a0_temp0 = (a1_temp0 * ConstV2 + a0_temp0 *ConstU2);
			a1_temp0 = (a1_temp0 * ConstV1);

			a2_temp1 = (a0_temp1 * ConstU1);
			a0_temp1 = (a1_temp1 * ConstV2 + a0_temp1 *ConstU2);
			a1_temp1 = (a1_temp1 * ConstV1);

			a4_temp0  = (ccClip255[((a3_temp0 + a1_temp0)>>20)]);
			a5_temp0  = (ccClip255[((a3_temp0 - a0_temp0)>>20)]);
			a3_temp0  = (ccClip255[((a3_temp0 + a2_temp0)>>20)]);
			temp0     = (a4_temp0<<16)|(a5_temp0<<8)|a3_temp0;

			a4_temp1  = (ccClip255[((a3_temp1 + a1_temp1)>>20)]);
			a5_temp1  = (ccClip255[((a3_temp1 - a0_temp1)>>20)]);
			a3_temp1  = (ccClip255[((a3_temp1 + a2_temp1)>>20)]);
			temp0    |= (a3_temp1 << 24);
			temp1     = (a4_temp1<<8)|a5_temp1;

			a3_temp0 = (psrc_y[(2*number + 1)] - 16)*ConstY;
			a3_temp1 = (psrc_y[(2*number + (in_stride << 1) + 1)] - 16)*ConstY;

			a5_temp0  = (ccClip255[((a7_temp0 + a2_temp0)>>20)]);
			temp1    |= (a5_temp0 << 16);
			a5_temp0  = (ccClip255[((a7_temp0 + a1_temp0)>>20)]);
			a7_temp0  = (ccClip255[((a7_temp0 - a0_temp0)>>20)]);
			temp1    |= (a7_temp0 << 24);
			temp2     =  a5_temp0;

			a5_temp1 = (ccClip255[((a7_temp1 + a2_temp1)>>20)]);
			temp2   |= (a5_temp1 << 8);
			a5_temp1 = (ccClip255[((a7_temp1 + a1_temp1)>>20)]);
			a7_temp1 = (ccClip255[((a7_temp1 - a0_temp1)>>20)]);
			a7_temp1|= (a5_temp1<<8);
			temp2   |= (a7_temp1 << 16);

			*((VO_S32*)(out_buf))     = (VO_S32)temp0;
			*((VO_S32*)(out_buf + 4)) = (VO_S32)temp1;
			*((VO_S32*)(out_buf + 8)) = (VO_S32)temp2;

			a4_temp0  = (ccClip255[((a3_temp0 + a1_temp0)>>20)]);
			a5_temp0  = (ccClip255[((a3_temp0 - a0_temp0)>>20)]);
			a3_temp0  = (ccClip255[((a3_temp0 + a2_temp0)>>20)]);
			temp0     = (a4_temp0<<16)|(a5_temp0<<8)|a3_temp0;

			a4_temp1  = (ccClip255[((a3_temp1 + a1_temp1)>>20)]);
			a5_temp1  = (ccClip255[((a3_temp1 - a0_temp1)>>20)]);
			a3_temp1  = (ccClip255[((a3_temp1 + a2_temp1)>>20)]);
			temp0    |= a3_temp1;
			temp1     = (a4_temp1<< 8)| a5_temp1;

			a7_temp0 = (psrc_y[(in_stride + 2*number + 1)] - 16)*ConstY;
			a7_temp1 = (psrc_y[(in_stride + 2*number+ (in_stride<<1) + 1)] - 16)*ConstY;

			a5_temp0  = (ccClip255[((a7_temp0 + a2_temp0)>>20)]);
			temp1    |= (a5_temp0 << 16);
			a5_temp0  = (ccClip255[((a7_temp0 + a1_temp0)>>20)]);
			a7_temp0  = (ccClip255[((a7_temp0 - a0_temp0)>>20)]);
			temp1    |= (a7_temp0 << 24);
			temp2     = a5_temp0;

			a5_temp1  = (ccClip255[((a7_temp1 + a2_temp1)>>20)]);
			temp2    |= (a5_temp1 << 8);
			a5_temp1  = (ccClip255[((a7_temp1 + a1_temp1)>>20)]);
			a7_temp1  = (ccClip255[((a7_temp1 - a0_temp1)>>20)]);
			a7_temp1 |= (a5_temp1 << 8);
			temp2    |= (a7_temp1 << 16);

			*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)temp0;
			*((VO_S32*)(out_buf+(-out_stride) + 4)) = (VO_S32)temp1;
			*((VO_S32*)(out_buf+(-out_stride) + 8)) = (VO_S32)temp2;
			out_buf+=-out_stride;
		}

		psrc_y += (in_stride<<2);
		psrc_u += (uin_stride<<1);
		psrc_v += (vin_stride<<1);
		out_buf+=-out_stride;
		out_buf+=12;					
		temp0 = out_stride<<4;
		out_buf += temp0;
		
	}while(--a6 != 0);
}

void cc_rgb24_mb_16x16_r90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
					   VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
					   const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 4;
	do{
		VO_S32 a0_temp0, a1_temp0, a2_temp0, a3_temp0, a4_temp0, a5_temp0, a7_temp0;
		VO_S32 a0_temp1, a1_temp1, a2_temp1, a3_temp1, a4_temp1, a5_temp1, a7_temp1;
		VO_S32 temp0, temp1, temp2;
		VO_S32 num;

		for(num = 0; num< 8; num++)
		{
			a0_temp0 = psrc_u[num] - 128;
			a1_temp0 = psrc_v[num] - 128;
			a3_temp0 = (psrc_y[2*num] - 16)*ConstY;
			a7_temp0 = (psrc_y[in_stride+2*num] - 16)*ConstY;

			a0_temp1 = psrc_u[num] - 128;
			a1_temp1 = psrc_v[num] - 128;
			a3_temp1 = (psrc_y[2*num] - 16)*ConstY;
			a7_temp1 = (psrc_y[in_stride+2*num+(in_stride<<1)] - 16)*ConstY;

			a2_temp0 = (a0_temp0 * ConstU1);
			a0_temp0 = (a1_temp0 * ConstV2 + a0_temp0 *ConstU2);
			a1_temp0 = (a1_temp0 * ConstV1);

			a2_temp1 = (a0_temp1 * ConstU1);
			a0_temp1 = (a1_temp1 * ConstV2 + a0_temp1 *ConstU2);
			a1_temp1 = (a1_temp1 * ConstV1);

			a4_temp0  = (ccClip255[((a3_temp0 + a1_temp0)>>20)]);
			a5_temp0  = (ccClip255[((a3_temp0 - a0_temp0)>>20)]);
			a3_temp0  = (ccClip255[((a3_temp0 + a2_temp0)>>20)]);
			temp2     = (a4_temp0<<24)|(a5_temp0<<16)|(a3_temp0<<8);

			a4_temp1  = (ccClip255[((a3_temp0 + a1_temp0)>>20)]);
			a5_temp1  = (ccClip255[((a3_temp0 - a0_temp0)>>20)]);
			a3_temp1  = (ccClip255[((a3_temp0 + a2_temp0)>>20)]);
			temp2    |= a4_temp1;
			temp1     = (a5_temp1<<24)|(a3_temp1<<16);

			a3_temp0 = (psrc_y[(2*num + 1)] - 16)*ConstY;
			a3_temp1 = (psrc_y[(2*num + in_stride + 1)] - 16)*ConstY;

			a5_temp0  = (ccClip255[((a7_temp0 + a2_temp0)>>20)]);
			temp0     = a5_temp0 << 24;//dif form left
			a5_temp0  = (ccClip255[((a7_temp0 + a1_temp0)>>20)]);
			a7_temp0  = (ccClip255[((a7_temp0 - a0_temp0)>>20)]);
			a7_temp0 |= (a5_temp0<<8);
			temp1    |=  a7_temp0;//dif form left

			a5_temp1  = (ccClip255[((a7_temp1 + a2_temp1)>>20)]);
			a4_temp1  = a5_temp0;//dif form left
			a5_temp1  = (ccClip255[((a7_temp1 + a1_temp1)>>20)]);
			a7_temp1  = (ccClip255[((a7_temp1 - a0_temp1)>>20)]);
			a7_temp1 |= (a5_temp1<<8);
			a4_temp1 |= (a7_temp1<<8);//dif form left
			temp0    |= a4_temp1;

			*((VO_S32*)(out_buf))     = (VO_S32)temp0;
			*((VO_S32*)(out_buf + 4)) = (VO_S32)temp1;
			*((VO_S32*)(out_buf + 8)) = (VO_S32)temp2;

			a4_temp0  = (ccClip255[((a3_temp0 + a1_temp0)>>20)]);
			a5_temp0  = (ccClip255[((a3_temp0 - a0_temp0)>>20)]);
			a3_temp0  = (ccClip255[((a3_temp0 + a2_temp0)>>20)]);
			temp2     = (a4_temp0<<24)|(a5_temp0<<16)|(a3_temp0<<8);

			a4_temp1  = (ccClip255[((a3_temp1 + a1_temp1)>>20)]);
			a5_temp1  = (ccClip255[((a3_temp1 - a0_temp1)>>20)]);
			a3_temp1  = (ccClip255[((a3_temp1 + a2_temp1)>>20)]);
			temp2    |= a4_temp1;
			temp1     = (a5_temp1<<24)|(a3_temp1<<16);

			a7_temp0 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;
			a7_temp1 = (psrc_y[(in_stride+2*num + 1 + (in_stride << 1))] - 16)*ConstY;

			a5_temp0  = (ccClip255[((a7_temp0 + a2_temp0)>>20)]);
			temp0     = a5_temp0 << 24;//dif form left
			a5_temp0  = (ccClip255[((a7_temp0 + a1_temp0)>>20)]);
			a7_temp0  = (ccClip255[((a7_temp0 - a0_temp0)>>20)]);
			a7_temp0 |= (a5_temp0<<8);
			temp1    |=  a7_temp0;//dif form left

			a5_temp1  = (ccClip255[((a7_temp1 + a2_temp1)>>20)]);
			a4_temp1 |= a5_temp0;
			a5_temp1  = (ccClip255[((a7_temp1 + a1_temp1)>>20)]);
			a7_temp1  = (ccClip255[((a7_temp1 - a0_temp1)>>20)]);
			a7_temp1 |= (a5_temp1<<8);
			a4_temp1 |= (a7_temp1<<8);
			temp0    |= a4_temp1;

			*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)temp0;
			*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)temp1;
			*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)temp2;
			out_buf+=out_stride;
		}

		psrc_y += (in_stride<<2);
		psrc_u += (uin_stride<<1);
		psrc_v += (vin_stride<<1);
		out_buf+=out_stride;
		out_buf-= 12;	
		temp0 = out_stride<<4;
		out_buf -= temp0;


	}while(--a6 != 0);
}

void cc_rgb24_mb_16x16_180_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
					   VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
					   const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	VO_U32 num;

	out_buf += (out_stride<<1) * 8 - out_stride;

	do{
		VO_S32 a0_temp0, a1_temp0, a2_temp0, a3_temp0, a4_temp0, a5_temp0, a7_temp0;
		VO_S32 a0_temp1, a1_temp1, a2_temp1, a3_temp1, a4_temp1, a5_temp1, a7_temp1;
		VO_S32 temp0, temp1, temp2;

		for(num = 0; num < 4; num++)
		{
			a0_temp0 = psrc_u[2*num] - 128;
			a1_temp0 = psrc_v[2*num] - 128;
			a3_temp0 = (psrc_y[4*num] - 16)*ConstY;
			a7_temp0 = (psrc_y[in_stride+2*num] - 16)*ConstY;
			a2_temp0 = (a0_temp0 * ConstU1);
			a0_temp0 = (a1_temp0 * ConstV2 + a0_temp0 *ConstU2);
			a1_temp0 = (a1_temp0 * ConstV1);

			a4_temp0  = (ccClip63[((a3_temp0 + a1_temp0)>>20)]);
			a5_temp0  = (ccClip63[((a3_temp0 - a0_temp0)>>20)]);
			a3_temp0  = (ccClip63[((a3_temp0 + a2_temp0)>>20)]);
			temp2     = (a4_temp0<<24)|(a5_temp0<<16)|(a3_temp0<<8);

			a3_temp0  = (psrc_y[(4*num + 1)] - 16)*ConstY;
			a5_temp0  = (ccClip63[((a3_temp0 + a2_temp0)>>20)]);
            temp1     = (a5_temp0 << 16); 
			a5_temp0  = (ccClip63[((a3_temp0 + a1_temp0)>>20)]);
			a3_temp0  = (ccClip63[((a3_temp0 - a0_temp0)>>20)]);
			temp2    |= a5_temp0;
			temp1    |= (a3_temp0<<24);

			a0_temp1 = psrc_u[2*num+1] - 128;
			a1_temp1 = psrc_v[2*num+1] - 128;
			a3_temp1 = (psrc_y[4*num + 2] - 16)*ConstY;
			a7_temp1 = (psrc_y[in_stride+ 4*num + 2] - 16)*ConstY;
			a2_temp1 = (a0_temp1 * ConstU1);
			a0_temp1 = (a1_temp1 * ConstV2 + a0_temp1 *ConstU2);
			a1_temp1 = (a1_temp1 * ConstV1);

			a4_temp1  = (ccClip63[((a3_temp1 + a1_temp1)>>20)]);
			a5_temp1  = (ccClip63[((a3_temp1 - a0_temp1)>>20)]);
			a3_temp1  = (ccClip63[((a3_temp1 + a2_temp1)>>20)]);
			temp1    |= (a4_temp1<<8)|(a5_temp1);
			temp0     = (a3_temp1<<24);

			a3_temp1 = (psrc_y[(4*num + 3)] - 16)*ConstY;
			a5_temp1 = (ccClip63[((a3_temp1 + a2_temp1)>>20)]);
			temp0   |= a5_temp1;
			a5_temp1 = (ccClip63[((a3_temp1 + a1_temp1)>>20)]);
			a3_temp1 = (ccClip63[((a3_temp1 - a0_temp1)>>20)]);
			a3_temp1|= (a5_temp1<<8);
			temp0   |= (a3_temp1<<8);


			*((VO_S32*)(out_buf+(4*(7 - num)))) = (VO_S32)temp0;
			*((VO_S32*)(out_buf+(4*(7 - num)))) = (VO_S32)temp1;
			*((VO_S32*)(out_buf+(4*(7 - num)))) = (VO_S32)temp2;

			a4_temp0  = (ccClip63[((a7_temp0 + a1_temp0)>>20)]);
			a5_temp0  = (ccClip63[((a7_temp0 - a0_temp0)>>20)]);
			a7_temp0  = (ccClip63[((a7_temp0 + a2_temp0)>>20)]);
			temp2 = (a4_temp0<<24)|(a5_temp0<<16)|(a7_temp0<<8);

			a7_temp0  = (psrc_y[(in_stride+4*num + 1)] - 16)*ConstY;
			a5_temp0  = (ccClip63[((a7_temp0 + a2_temp0)>>20)]);
			temp1     = (a5_temp0 << 16);
			a5_temp0  = (ccClip63[((a7_temp0 + a1_temp0)>>20)]);
			a7_temp0  = (ccClip63[((a7_temp0 - a0_temp0)>>20)]);
			temp2    |= a5_temp0;
			temp1    |= (a7_temp0<<24);

			a4_temp1  = (ccClip63[((a7_temp1 + a1_temp1)>>20)]);
			a5_temp1  = (ccClip63[((a7_temp1 - a0_temp1)>>20)]);
			a7_temp1  = (ccClip63[((a7_temp1 + a2_temp1)>>20)]);
			temp1    |= (a4_temp1<<8)|(a5_temp1);
			temp0     = (a7_temp1<<24);

			a7_temp1  = (psrc_y[(in_stride+4*num + 3)] - 16)*ConstY;
			a5_temp1  = (ccClip63[((a7_temp1 + a2_temp1)>>20)]);
			temp0    |= (a5_temp1);
			a5_temp1  = (ccClip63[((a7_temp1 + a1_temp1)>>20)]);
			a7_temp1  = (ccClip63[((a7_temp1 - a0_temp1)>>20)]);
			a7_temp1 |= (a5_temp1<<8);
			temp0    |= (a7_temp1<<8);


			*((VO_S32*)(out_buf+(4*(7 - num) - out_stride))) = (VO_S32)temp0;
			*((VO_S32*)(out_buf+(4*(7 - num) - out_stride))) = (VO_S32)temp1;
			*((VO_S32*)(out_buf+(4*(7 - num) - out_stride))) = (VO_S32)temp2;
		} 

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}

VOCCRETURNCODE NewYUVPlanarToRGB24_normal(ClrConvData *conv_data, const VO_U32 isRGB565, const VO_U32 is420Planar,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	const VO_U32 in_stride = conv_data->nInStride , out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	const VO_U32 in_uvstride = conv_data->nInUVStride;
	//const VO_S32 *param = cc_hnd->yuv2rgbmatrix;
	VO_U8 *outbuf, *start_outbuf=NULL;
	VO_U8 *psrcY, *psrcU, *psrcV;
	VO_S32 step_y=0, step_x=0, out_stride = conv_data->nOutStride;

	/*start of output address*/
	if(conv_data->nRotationType == ROTATION_DISABLE){/*disable rotation*/
		step_y = 3;
		step_x = out_stride;
		start_outbuf = conv_data->pOutBuf[0];
	}else if (conv_data->nRotationType == ROTATION_180){/*rotation180*/
		step_y = -3;
		step_x = -out_stride;
		start_outbuf = conv_data->pOutBuf[0];
	}else if(conv_data->nRotationType == ROTATION_90L){/*rotation90L*/
		step_y = -out_stride;
		step_x = 3;
		start_outbuf = conv_data->pOutBuf[0] - (out_width - 1)* step_y;
	}else if(conv_data->nRotationType == ROTATION_90R){/*rotation90R*/
		step_x = -3;
		step_y = out_stride;
		start_outbuf = conv_data->pOutBuf[0] + (out_height<<1) - 2 ;
	}

	for(vy = 0; vy < out_height; vy++) {
		outbuf = start_outbuf;
		vx =  vy*in_stride;
		psrcY = conv_data->pInBuf[0] + vx;
		vx = (vy>>is420Planar)*in_uvstride;
		psrcU = conv_data->pInBuf[1] + vx;
		psrcV = conv_data->pInBuf[2] + vx;

		for(vx = 0; vx < out_width; vx++) {
			int a0, a1, a2, a3;

			a3 = psrcV[vx>>1] - 128;
			a2 = psrcU[vx>>1] - 128;

			a0 = a3 * ConstV1;
			a1 = a3 * ConstV2 + a2 *ConstU2;
			a2 = a2 * ConstU1;
			a3 = (psrcY[vx] - 16)*ConstY;

			*(outbuf)   = ccClip255[(a3 + a2 )>>20];  
			*(outbuf+1) = ccClip255[(a3 - a1 )>>20]; 
			*(outbuf+2) = ccClip255[(a3 + a0 )>>20];

			outbuf += step_y;
		}

		start_outbuf += step_x;
	}	

	return VO_ERR_NONE;
}
#endif //COLCONVENABLE

#if 0
void cc_rgb32_mb_16x16_l90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
							 VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
							 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7,a8;
		VO_S32 number;
		//0
		number = 0;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);

		*((VO_S32*)(out_buf)) = (VO_S32)a4;
		*((VO_S32*)(out_buf + 4)) = (VO_S32)a5;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		number = 1;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7 <<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		number = 2;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7 << 8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5|= (a7 <<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		number = 3;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7 << 8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7 << 8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		number = 4;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7 <<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7 << 8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		number = 5;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7 << 8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7 << 8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		number = 6;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7 << 8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7 << 8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		number = 7;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7 << 8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7 << 8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a5;
		out_buf+=-out_stride;

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf+=-out_stride;
		out_buf+=8;					
		a7 = out_stride<<4;
		out_buf += a7;

	}while(--a6 != 0);
}

void cc_rgb32_mb_16x16_r90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
							 VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
							 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7,a8;
		VO_S32 num;

		num = 0;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);//dif form left

		*((VO_S32*)(out_buf+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf)) = (VO_S32)a5;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride)))   = (VO_S32)a5;
		out_buf+=out_stride;

		num = 1;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a5;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride)))   = (VO_S32)a5;
		out_buf+=out_stride;

		num = 2;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);//dif form left

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride)))   = (VO_S32)a5;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a5;
		out_buf+=out_stride;

		num = 3;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);//dif form left

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a5;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a5;
		out_buf+=out_stride;

		num = 4;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a5;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a5;
		out_buf+=out_stride;

		num = 5;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);//dif form left

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a5;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a5;
		out_buf+=out_stride;

		num = 6;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);//dif form left

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a5;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a5;
		out_buf+=out_stride;

		num = 7;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a5;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a4<<16)|(a5<<8)|a3;

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a8<<8);
		a5 |= (a7<<8);
		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a5;
		out_buf+=out_stride;

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf+=out_stride;
		out_buf-= 8;	
		a7 = out_stride<<4;
		out_buf -= a7;

	}while(--a6 != 0);
}

#else
void cc_rgb32_mb_16x16_l90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
							 VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
							 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7,a8;

		//0
		NEWLOAD_DATA(a3, a7, 0, 0)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 1)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90L_PIX(a4,a5,0)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+1)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//1
		NEWLOAD_DATA(a3, a7, 1, 2)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3,3)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+3)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//2
		NEWLOAD_DATA(a3, a7, 2, 4)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 5)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+5)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//3
		NEWLOAD_DATA(a3, a7, 3, 6)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 7)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+7)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//4
		NEWLOAD_DATA(a3, a7, 4, 8)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 9)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+9)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//5
		NEWLOAD_DATA(a3, a7, 5, 10)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 11)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+11)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//6
		NEWLOAD_DATA(a3, a7, 6, 12)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 13)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+13)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//7
		NEWLOAD_DATA(a3, a7, 7, 14)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 15)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+15)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf+=-out_stride;
		out_buf+=8;					
		a7 = out_stride<<4;
		out_buf += a7;

	}while(--a6 != 0);
}

void cc_rgb32_mb_16x16_r90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
							 VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
							 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7,a8;

		//0
		NEWLOAD_DATA(a3, a7, 0, 0)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 1)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90R_PIX(a4,a5,0)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+1)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//1
		NEWLOAD_DATA(a3, a7, 1, 2)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3,3)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+3)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//2
		NEWLOAD_DATA(a3, a7, 2, 4)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 5)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+5)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//3
		NEWLOAD_DATA(a3, a7, 3, 6)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 7)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+7)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//4
		NEWLOAD_DATA(a3, a7, 4, 8)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 9)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+9)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//5
		NEWLOAD_DATA(a3, a7, 5, 10)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 11)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+11)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//6
		NEWLOAD_DATA(a3, a7, 6, 12)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 13)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+13)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//7
		NEWLOAD_DATA(a3, a7, 7, 14)
		NEWCAL_UV()
		NEWPIXRGB32_0(a3)
		NEWLOAD_YPIX(a3, 15)
		NEWPIXRGB32_1(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXRGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+15)
		NEWPIXRGB32_3(a7)

		WRITERGB32_90R_PIX(a4,a5,out_stride)

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf+=out_stride;
		out_buf-= 8;	
		a7 = out_stride<<4;
		out_buf -= a7;

	}while(--a6 != 0);
}

#endif

void cc_rgb32_mb_16x16_180_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
					         VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
					         const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8,num;
	out_buf += (out_stride<<1) * 8 - out_stride;

	do{
		VO_S32 a0, a1, a2, a3, a5, a7,a8;
		VO_U32 a4;

		for(num = 0; num < 8; num++)
		{
			a0 = psrc_u[num] - 128;
			a1 = psrc_v[num] - 128;
			a3 = (psrc_y[2*num] - 16)*ConstY;
			a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;
			a2 = (a0 * ConstU1);
			a0 = (a1 * ConstV2 + a0 *ConstU2);
			a1 = (a1 * ConstV1);

			a4  = (ccClip255[((a3 + a1)>>20)]);
			a5  = (ccClip255[((a3 - a0)>>20)]);
			a3  = (ccClip255[((a3 + a2)>>20)]);
			a4 = (a4<<16)|(a5<<8)|a3;

			a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;
			a5  = (ccClip255[((a3 + a2)>>20)]);
			a8  = (ccClip255[((a3 + a1)>>20)]);

			a3  = (ccClip255[((a3 - a0)>>20)]);
			a3 |= (a8<<8);
			a5 |= (a3<<8);
			*((VO_S32*)(out_buf+(8*(7 - num)+4)))     = (VO_S32)a4;
			*((VO_S32*)(out_buf+(8*(7 - num)))) = (VO_S32)a5;

			a4  = (ccClip255[((a7 + a1)>>20)]);
			a5  = (ccClip255[((a7 - a0)>>20)]);
			a7  = (ccClip255[((a7 + a2)>>20)]);
			a4 = (a4<<16)|(a5<<8)|a7;
			a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;
			a5  = (ccClip255[((a7 + a2)>>20)]);
			a8  = (ccClip255[((a7 + a1)>>20)]);
			a7  = (ccClip255[((a7 - a0)>>20)]);
			a7 |= (a8<<8);
			a5 |= (a7<<8);

			*((VO_S32*)(out_buf+(8*(7 - num) + 4 - out_stride)))     = (VO_S32)a4;
			*((VO_S32*)(out_buf+((8*(7 - num)) - out_stride))) = (VO_S32)a5;
		} 
		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}


VOCCRETURNCODE NewYUVPlanarToRGB32_normal(ClrConvData *conv_data, const VO_U32 isRGB565, const VO_U32 is420Planar,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	const VO_U32 in_stride = conv_data->nInStride , out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	const VO_U32 in_uvstride = conv_data->nInUVStride;
	//const VO_S32 *param = cc_hnd->yuv2rgbmatrix;
	VO_U8 *outbuf, *start_outbuf=NULL;
	VO_U8 *psrcY, *psrcU, *psrcV;
	VO_S32 step_y=0, step_x=0, out_stride = conv_data->nOutStride;

	/*start of output address*/
	if(conv_data->nRotationType == ROTATION_DISABLE){/*disable rotation*/
		step_y = 4;
		step_x = out_stride;
		start_outbuf = conv_data->pOutBuf[0];
	}else if (conv_data->nRotationType == ROTATION_180){/*rotation180*/
		step_y = -4;
		step_x = -out_stride;
		start_outbuf = conv_data->pOutBuf[0] -4;
	}else if(conv_data->nRotationType == ROTATION_90L){/*rotation90L*/
		step_y = -out_stride;
		step_x = 4;
		start_outbuf = conv_data->pOutBuf[0] - (out_width - 1)* step_y;
	}else if(conv_data->nRotationType == ROTATION_90R){/*rotation90R*/
		step_x = -4;
		step_y = out_stride;
		start_outbuf = conv_data->pOutBuf[0] + (out_height<<2) - 4 ;
	}

	for(vy = 0; vy < out_height; vy++) {
		outbuf = start_outbuf;
		vx =  vy*in_stride;
		psrcY = conv_data->pInBuf[0] + vx;
		vx = (vy>>is420Planar)*in_uvstride;
		psrcU = conv_data->pInBuf[1] + vx;
		psrcV = conv_data->pInBuf[2] + vx;

		for(vx = 0; vx < out_width; vx++) {
			VO_S32 a0, a1, a2, a3;

			a3 = psrcV[vx>>1] - 128;
			a2 = psrcU[vx>>1] - 128;

			a0 = a3 * ConstV1;
			a1 = a3 * ConstV2 + a2 *ConstU2;
			a2 = a2 * ConstU1;
			a3 = (psrcY[vx] - 16)*ConstY;


			a0  = ccClip255[(a3 + a0 )>>20];
			a1  = ccClip255[(a3 - a1 )>>20];
			a2  = ccClip255[(a3 + a2 )>>20];

			a0 = ((((a0<<8)|a1 )<<8))|a2;

			*((VO_S32*)outbuf) = a0;
			outbuf += step_y;
		}
		start_outbuf += step_x;
	}	
	return VO_ERR_NONE;
}

#if 0

void cc_argb32_mb_16x16_l90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
							  VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
							  const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7,a8;
		VO_S32 number;
		//0
		number = 0;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf)) = (VO_S32)a4;
		*((VO_S32*)(out_buf + 4)) = (VO_S32)a8;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		number = 1;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		number = 2;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		number = 3;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		number = 4;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		number = 5;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		number = 6;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		number = 7;
		a0 = psrc_u[number] - 128;
		a1 = psrc_v[number] - 128;
		a3 = (psrc_y[2*number] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*number] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*number + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(-out_stride)+4)) = (VO_S32)a8;
		out_buf+=-out_stride;

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf+=-out_stride;
		out_buf+=8;					
		a7 = out_stride<<4;
		out_buf += a7;

	}while(--a6 != 0);
}

void cc_argb32_mb_16x16_r90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
							  VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
							  const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7,a8;
		VO_S32 num;

		num = 0;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);//dif form left


		*((VO_S32*)(out_buf+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf)) = (VO_S32)a8;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride)))   = (VO_S32)a8;
		out_buf+=out_stride;

		num = 1;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a8;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride)))   = (VO_S32)a8;
		out_buf+=out_stride;

		num = 2;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride)))   = (VO_S32)a8;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a8;
		out_buf+=out_stride;

		num = 3;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a8;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a8;
		out_buf+=out_stride;

		num = 4;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a8;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a8;
		out_buf+=out_stride;

		num = 5;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a8;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a8;
		out_buf+=out_stride;

		num = 6;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a8;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a8;
		out_buf+=out_stride;

		num = 7;
		a0 = psrc_u[num] - 128;
		a1 = psrc_v[num] - 128;
		a3 = (psrc_y[2*num] - 16)*ConstY;
		a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;

		a2 = (a0 * ConstU1);
		a0 = (a1 * ConstV2 + a0 *ConstU2);
		a1 = (a1 * ConstV1);

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);

		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a8;
		out_buf+=out_stride;

		a4  = (ccClip255[((a3 + a1)>>20)]);
		a5  = (ccClip255[((a3 - a0)>>20)]);
		a3  = (ccClip255[((a3 + a2)>>20)]);
		a4 = (a3<<16)|(a5<<8)|(a4);

		a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

		a5  = (ccClip255[((a7 + a2)>>20)]);
		a8  = (ccClip255[((a7 + a1)>>20)]);
		a7  = (ccClip255[((a7 - a0)>>20)]);
		a7 |= (a5<<8);
		a8 |= (a7<<8);
		*((VO_S32*)(out_buf+(out_stride)+4)) = (VO_S32)a4;
		*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a8;
		out_buf+=out_stride;

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf+=out_stride;
		out_buf-= 8;	
		a7 = out_stride<<4;
		out_buf -= a7;

	}while(--a6 != 0);
}

#else
void cc_argb32_mb_16x16_l90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
							  VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
							  const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7,a8;

		//0
		NEWLOAD_DATA(a3, a7, 0, 0)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 1)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,0)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+1)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//1
		NEWLOAD_DATA(a3, a7, 1, 2)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 3)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+3)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//2
		NEWLOAD_DATA(a3, a7, 2, 4)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3,5)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+5)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//3
		NEWLOAD_DATA(a3, a7, 3, 6)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3,7)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+7)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//4
		NEWLOAD_DATA(a3, a7, 4, 8)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3,9)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+9)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//5
		NEWLOAD_DATA(a3, a7, 5, 10)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 11)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+11)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//6
		NEWLOAD_DATA(a3, a7, 6, 12)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3,13)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+13)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		//7
		NEWLOAD_DATA(a3, a7, 7, 14)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3,15)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+15)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90L_PIX(a4,a5,-out_stride)

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf+=-out_stride;
		out_buf+=8;					
		a7 = out_stride<<4;
		out_buf += a7;

	}while(--a6 != 0);
}

void cc_argb32_mb_16x16_r90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
							  VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
							  const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7,a8;

		//0
		NEWLOAD_DATA(a3, a7, 0, 0)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 1)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,0)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+1)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//1
		NEWLOAD_DATA(a3, a7, 1, 2)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 3)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+3)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//2
		NEWLOAD_DATA(a3, a7, 2, 4)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3,5)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+5)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//3
		NEWLOAD_DATA(a3, a7, 3, 6)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3,7)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+7)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//4
		NEWLOAD_DATA(a3, a7, 4, 8)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3,9)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+9)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//5
		NEWLOAD_DATA(a3, a7, 5, 10)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3, 11)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+11)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//6
		NEWLOAD_DATA(a3, a7, 6, 12)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3,13)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+13)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)

		//7
		NEWLOAD_DATA(a3, a7, 7, 14)
		NEWCAL_UV()
		NEWPIXARGB32_0(a3)
		NEWLOAD_YPIX(a3,15)
		NEWPIXARGB32_1(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)
		NEWPIXARGB32_2(a3)
		NEWLOAD_YPIX(a7, in_stride+15)
		NEWPIXARGB32_3(a7)

		a4 |= (255 << 24);
		a5 |= (255 << 24);
		WRITERGB32_90R_PIX(a4,a5,out_stride)

		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf+=out_stride;
		out_buf-= 8;	
		a7 = out_stride<<4;
		out_buf -= a7;

	}while(--a6 != 0);
}
#endif //0
void cc_argb32_mb_16x16_180_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
							  VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
							  const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8,num;
	out_buf += (out_stride<<1) * 8 - out_stride;

	do{
		VO_S32 a0, a1, a2, a3, a5, a7,a8;
		VO_U32 a4;

		for(num = 0; num < 8; num++)
		{
			a0 = psrc_u[num] - 128;
			a1 = psrc_v[num] - 128;
			a3 = (psrc_y[2*num] - 16)*ConstY;
			a7 = (psrc_y[in_stride+2*num] - 16)*ConstY;
			a2 = (a0 * ConstU1);
			a0 = (a1 * ConstV2 + a0 *ConstU2);
			a1 = (a1 * ConstV1);

			a4  = (ccClip255[((a3 + a1)>>20)]);
			a5  = (ccClip255[((a3 - a0)>>20)]);
			a3  = (ccClip255[((a3 + a2)>>20)]);
			a4  = (255<<24)|(a3<<16)|(a5<<8)|(a4);

			a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;
			a5  = (ccClip255[((a3 + a2)>>20)]);
			a8  = (ccClip255[((a3 + a1)>>20)]);

			a3  = (ccClip255[((a3 - a0)>>20)]);
			a3 |= (a5<<8); 
			a8 |= (a3<<8);
			a8 |= (255<<24);
			*((VO_S32*)(out_buf+(8*(7 - num)+4))) = (VO_S32)a4;
			*((VO_S32*)(out_buf+(8*(7 - num))))   = (VO_S32)a8;

			a4  = (ccClip255[((a7 + a1)>>20)]);
			a5  = (ccClip255[((a7 - a0)>>20)]);
			a7  = (ccClip255[((a7 + a2)>>20)]);
			a4  =  (255<<24)|(a3<<16)|(a5<<8)|(a4);
			a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;
			a5  = (ccClip255[((a7 + a2)>>20)]);
			a8  = (ccClip255[((a7 + a1)>>20)]);
			a7  = (ccClip255[((a7 - a0)>>20)]);
			a7 |= (a5<<8);
			a8 |= (a7<<8);
			a8 |= (255<<24);

			*((VO_S32*)(out_buf+(8*(7 - num) + 4 - out_stride)))  = (VO_S32)a4;
			*((VO_S32*)(out_buf+((8*(7 - num)) - out_stride)))    = (VO_S32)a8;
		} 
		psrc_y += (in_stride<<1);
		psrc_u += uin_stride;
		psrc_v += vin_stride;
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}

VOCCRETURNCODE NewYUVPlanarToARGB32_normal(ClrConvData *conv_data, const VO_U32 isRGB565, const VO_U32 is420Planar,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	const VO_U32 in_stride = conv_data->nInStride , out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	const VO_U32 in_uvstride = conv_data->nInUVStride;
	//const VO_S32 *param = cc_hnd->yuv2rgbmatrix;
	VO_U8 *outbuf, *start_outbuf=NULL;
	VO_U8 *psrcY, *psrcU, *psrcV;
	VO_S32 step_y=0, step_x=0, out_stride = conv_data->nOutStride;

	/*start of output address*/
	if(conv_data->nRotationType == ROTATION_DISABLE){/*disable rotation*/
		step_y = 4;
		step_x = out_stride;
		start_outbuf = conv_data->pOutBuf[0];
	}else if (conv_data->nRotationType == ROTATION_180){/*rotation180*/
		step_y = -4;
		step_x = -out_stride;
		start_outbuf = conv_data->pOutBuf[0] -4;
	}else if(conv_data->nRotationType == ROTATION_90L){/*rotation90L*/
		step_y = -out_stride;
		step_x = 4;
		start_outbuf = conv_data->pOutBuf[0] - (out_width - 1)* step_y;
	}else if(conv_data->nRotationType == ROTATION_90R){/*rotation90R*/
		step_x = -4;
		step_y = out_stride;
		start_outbuf = conv_data->pOutBuf[0] + (out_height<<2) - 4 ;
	}

	for(vy = 0; vy < out_height; vy++) {
		outbuf = start_outbuf;
		vx =  vy*in_stride;
		psrcY = conv_data->pInBuf[0] + vx;
		vx = (vy>>is420Planar)*in_uvstride;
		psrcU = conv_data->pInBuf[1] + vx;
		psrcV = conv_data->pInBuf[2] + vx;

		for(vx = 0; vx < out_width; vx++) {
			VO_S32 a0, a1, a2, a3;

			a3 = psrcV[vx>>1] - 128;
			a2 = psrcU[vx>>1] - 128;

			a0 = a3 * ConstV1;
			a1 = a3 * ConstV2 + a2 *ConstU2;
			a2 = a2 * ConstU1;
			a3 = (psrcY[vx] - 16)*ConstY;


			a0  = ccClip255[(a3 + a0 )>>20];
			a1  = ccClip255[(a3 - a1 )>>20];
			a2  = ccClip255[(a3 + a2 )>>20];

			a0 = ((((a2<<8)|a1 )<<8))|a0|(255 << 24);

			*((VO_S32*)outbuf) = a0;
			outbuf += step_y;
		}
		start_outbuf += step_x;
	}	
	return VO_ERR_NONE;
}

void cc_yuv420_mb_s_l90_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
							   VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			a4 = src_y0[a6];
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			x_scale_par0 += 6;

		}while((i-=2) != 0);

		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_yuv420_mb_s_r90_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
							   VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			a4 = src_y0[a6];
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			x_scale_par0 += 6;

		}while((i-=2) != 0);

		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_yuv420_rgb32_mb_rotation_90l_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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
			VO_S32 temp0,temp1;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];		

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a2 -= 128;
			a3 -= 128;

			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			temp0 = ((ccClip255[(((a4 + a0)>>20) + RDITHERNEW_P0)])<<16)|((ccClip255[(((a4 - a1)>>20) + GDITHER_P0)])<<8)|(ccClip255[(((a4 + a2)>>20) + BDITHERNEW_P0)]) | ( alpha_value << 24 );	
			temp1 = (((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P1)])<<16)|((ccClip255[(((a5 - a1)>>20) + GDITHER_P1)])<<8)|(ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P1)])) | ( alpha_value << 24 );

			*((VO_U32*)dst)   = temp0;
			*((VO_U32*)(dst+4)) = temp1;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			temp0 = ((ccClip255[(((a4 + a0)>>20) + RDITHERNEW_P2)])<<16)|((ccClip255[(((a4 - a1)>>20) + GDITHER_P2)])<<8)|(ccClip255[(((a4 + a2)>>20) + BDITHERNEW_P2)]) | ( alpha_value << 24 );	
			temp1 = (((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P3)])<<16)|((ccClip255[(((a5 - a1)>>20) + GDITHER_P3)])<<8)|(ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P3)])) | ( alpha_value << 24 );

			*((VO_U32*)dst)     = temp0;
			*((VO_U32*)(dst + 4)) = temp1;
			dst -= dst_stride;

		}while((i-=2) != 0);

		dst += (width*dst_stride);
		dst += 8;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_yuv420_rgb32_mb_rotation_90l_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
											  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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
			VO_S32 temp0,temp1;
			// u, v
			a0 = x_scale_par0[0];//src pos0

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];
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

			temp0 = ((ccClip255[(a4 + a0)>>20])<<16)|((ccClip255[(a4 - a1)>>20])<<8)|(ccClip255[(a4 + a2)>>20]) | ( alpha_value << 24 );	
			temp1 = (((ccClip255[(a5 + a0)>>20])<<16)|((ccClip255[(a5 - a1)>>20])<<8)|(ccClip255[(a5 + a2)>>20])) | ( alpha_value << 24 );

			*((VO_U32*)dst)   = temp0;
			*((VO_U32*)(dst+4)) = temp1;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			x_scale_par0 += 3;

			a4 = src_y0[a6];
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			temp0 = ((ccClip255[(a4 + a0)>>20])<<16)|((ccClip255[(a4 - a1)>>20])<<8)|(ccClip255[(a4 + a2)>>20]) | ( alpha_value << 24 );	
			temp1 = (((ccClip255[(a5 + a0)>>20])<<16)|((ccClip255[(a5 - a1)>>20])<<8)|(ccClip255[(a5 + a2)>>20])) | ( alpha_value << 24 );

			*((VO_U32*)dst)     = temp0;
			*((VO_U32*)(dst + 4)) = temp1;
			dst -= dst_stride;

		}while((i-=2) != 0);

		dst += (width*dst_stride);
		dst += 8;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_yuv420_argb32_mb_rotation_90l_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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
			VO_S32 temp0,temp1;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];
			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];		

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a2 -= 128;
			a3 -= 128;

			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			temp0 = ((ccClip255[(((a4 + a0)>>20) + RDITHERNEW_P0)]))|((ccClip255[(((a4 - a1)>>20) + GDITHER_P0)])<<8)|((ccClip255[(((a4 + a2)>>20) + BDITHERNEW_P0)])<<16)|(255 << 24);	
			temp1 = ((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P1)]))|((ccClip255[(((a5 - a1)>>20) + GDITHER_P1)])<<8)|((ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P1)])<<16)|(255 << 24);

			*((VO_U32*)dst)   = temp0;
			*((VO_U32*)(dst+4)) = temp1;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			temp0 = ((ccClip255[(((a4 + a0)>>20) + RDITHERNEW_P2)]))|((ccClip255[(((a4 - a1)>>20) + GDITHER_P2)])<<8)|((ccClip255[(((a4 + a2)>>20) + BDITHERNEW_P2)])<<16)|(255 << 24);	
			temp1 = ((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P3)]))|((ccClip255[(((a5 - a1)>>20) + GDITHER_P3)])<<8)|((ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P3)])<<16)|(255 << 24);

			*((VO_U32*)dst)     = temp0;
			*((VO_U32*)(dst + 4)) = temp1;
			dst -= dst_stride;

		}while((i-=2) != 0);

		dst += (width*dst_stride);
		dst += 8;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_yuv420_argb32_mb_rotation_90l_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
										       VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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
			VO_S32 temp0,temp1;
			// u, v
			a0 = x_scale_par0[0];//src pos0

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];
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

			temp0 = ((ccClip255[(a4 + a0)>>20]))|((ccClip255[(a4 - a1)>>20])<<8)|((ccClip255[(a4 + a2)>>20])<<16)|(255 << 24);	
			temp1 = ((ccClip255[(a5 + a0)>>20]))|((ccClip255[(a5 - a1)>>20])<<8)|((ccClip255[(a5 + a2)>>20])<<16)|(255 << 24);

			*((VO_U32*)dst)   = temp0;
			*((VO_U32*)(dst+4)) = temp1;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			x_scale_par0 += 3;
			a4 = src_y0[a6];
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			temp0 = ((ccClip255[(a4 + a0)>>20]))|((ccClip255[(a4 - a1)>>20])<<8)|((ccClip255[(a4 + a2)>>20])<<16)|(255 << 24);	
			temp1 = ((ccClip255[(a5 + a0)>>20]))|((ccClip255[(a5 - a1)>>20])<<8)|((ccClip255[(a5 + a2)>>20])<<16)|(255 << 24);

			*((VO_U32*)dst)     = temp0;
			*((VO_U32*)(dst + 4)) = temp1;
			dst -= dst_stride;

		}while((i-=2) != 0);

		dst += (width*dst_stride);
		dst += 8;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_yuv420_mb_rotation_90l_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
								   VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, 
								   const VO_S32 vin_stride)
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

			a2 = src_u0[a1];
			a3 = src_v0[a1];		

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a2 -= 128;
			a3 -= 128;

			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

		}while((i-=2) != 0);

		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_yuv420_mb_s_l90_c_3quarter(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,
								   VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;

	do{
		//y0
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		VO_S32 first1 = 0x155;//y_scale_par[4];
		VO_S32 first2 = 0x2ab;//y_scale_par[5];
		VO_S32 second1 = 0x2aa;//x_scale_par0[1];
		VO_S32 second2 = 0x156;//x_scale_par0[2];	

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//			x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+1]*first1)>>10;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*first2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = src_y0[a6];
			a5 = (src_y1[a6]*first2 + src_y1[a6+src_stride]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+1]*second1)>>10;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*first2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;

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

		src_y1 = src_y + y_scale_par[3] * src_stride;


		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;

			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*second2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a6]*first2 + src_y1[a6+1]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+src_stride]*second1)>>10;
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*second2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a6]*second2 + src_y1[a6+1]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;

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

		src_y1 = src_y + y_scale_par[3] * src_stride;

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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*first2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*second2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+src_stride]*first1)>>10;
			a5 = (src_y1[a6]*second2 + src_y1[a6+src_stride]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*first2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*second2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;

			x_scale_par0 += 18;			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

		}while((i-=6) != 0);

		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;

	}while((height-=6) != 0);
}

void cc_yuv420_mb_s_r90_c_3quarter(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
								   VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;

	do{
		//y0
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		VO_S32 first1 = 0x155;//y_scale_par[4];
		VO_S32 first2 = 0x2ab;//y_scale_par[5];
		VO_S32 second1 = 0x2aa;//x_scale_par0[1];
		VO_S32 second2 = 0x156;//x_scale_par0[2];	

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+1]*first1)>>10;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*first2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


			*((VO_U32*)dst) = a4;
			dst += dst_stride;

			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = src_y0[a6];
			a5 = (src_y1[a6]*first2 + src_y1[a6+src_stride]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+1]*second1)>>10;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*first2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);

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
		src_y1 = src_y + y_scale_par[3] * src_stride;

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;

			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*second2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a6]*first2 + src_y1[a6+1]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


			*((VO_U32*)dst) = a4;
			dst += dst_stride;

			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+src_stride]*second1)>>10;
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*second2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a6]*second2 + src_y1[a6+1]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);

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
		src_y1 = src_y + y_scale_par[3] * src_stride;

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*first2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*second2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


			*((VO_U32*)dst) = a4;
			dst += dst_stride;

			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+src_stride]*first1)>>10;
			a5 = (src_y1[a6]*second2 + src_y1[a6+src_stride]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*first2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*second2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);

			x_scale_par0 += 18;			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;

		}while((i-=6) != 0);

		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;

	}while((height-=6) != 0);
}

void cc_yuv420_mb_s_l90_c_1point5(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,VO_S32 *x_scale_par, 
								  VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;

	do{
		//y0
		VO_S32 *x_scale_par0 = x_scale_par;
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		VO_S32 second1 = 0x155;//y_scale_par[4];
		VO_S32 second2= 0x2ab;//y_scale_par[5];
		VO_S32 first1= 0x2aa;//x_scale_par0[1];
		VO_S32 first2 = 0x156;//x_scale_par0[2];	

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+1]*first1)>>10;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*first2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

			//x1
			a0 = x_scale_par0[6];
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = src_y0[a6];
			a5 = (src_y1[a6]*first2 + src_y1[a6+src_stride]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+1]*second1)>>10;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*first2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;

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
		src_y1 = src_y + y_scale_par[3] * src_stride;

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;

			//x0
			a0 = x_scale_par0[0];
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*second2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a6]*first2 + src_y1[a6+1]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+src_stride]*second1)>>10;
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*second2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a6]*second2 + src_y1[a6+1]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;

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
		src_y1 = src_y + y_scale_par[3] * src_stride;

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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*first2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*second2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+src_stride]*first1)>>10;
			a5 = (src_y1[a6]*second2 + src_y1[a6+src_stride]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*first2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*second2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;

			x_scale_par0 += 18;			
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;

		}while((i-=6) != 0);

		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;

	}while((height-=6) != 0);
}



void cc_yuv420_mb_s_r90_c_1point5(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v,  VO_U8 *dst, VO_S32 src_stride, 
								  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;

	do{
		//y0
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;
		VO_S32 second1 = 0x155;//y_scale_par[4];
		VO_S32 second2= 0x2ab;//y_scale_par[5];
		VO_S32 first1= 0x2aa;//x_scale_par0[1];
		VO_S32 first2 = 0x156;//x_scale_par0[2];	

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+1]*first1)>>10;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*first2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


			*((VO_U32*)dst) = a4;
			dst += dst_stride;

			//x1
			a0 = x_scale_par0[6];
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = src_y0[a6];
			a5 = (src_y1[a6]*first2 + src_y1[a6+src_stride]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			//x2
			a0 = x_scale_par0[12];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+1]*second1)>>10;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*first2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*first1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);

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
		src_y1 = src_y + y_scale_par[3] * src_stride;

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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			//aa = 0x155;////x_scale_par0[1];
			//bb = 0x2ab;//x_scale_par0[2];

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*second2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*second1)>>20;
			a5 = (src_y1[a6]*first2 + src_y1[a6+1]*first1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


			*((VO_U32*)dst) = a4;
			dst += dst_stride;

			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*second2 + src_y0[a6+src_stride]*second1)>>10;
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*second2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*second1)>>20;
			a5 = (src_y1[a6]*second2 + src_y1[a6+1]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);

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
		src_y1 = src_y + y_scale_par[3] * src_stride;

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			//x0
			a0 = x_scale_par0[0];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*first2 + src_y0[a6+1]*first1)*first2 + (src_y0[a6+src_stride]*first2 + src_y0[a6+src_stride+1]*first1)*first1)>>20;
			a5 = ((src_y1[a6]*first2 + src_y1[a6+1]*first1)*second2 + (src_y1[a6+src_stride]*first2 + src_y1[a6+src_stride+1]*first1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


			*((VO_U32*)dst) = a4;
			dst += dst_stride;

			//x1
			a0 = x_scale_par0[6];

			//x_scale_par0 += 3;
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = (src_y0[a6]*first2 + src_y0[a6+src_stride]*first1)>>10;
			a5 = (src_y1[a6]*second2 + src_y1[a6+src_stride]*second1)>>10;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			//x_scale_par0 += 3;
			a4 = ((src_y0[a6]*second2 + src_y0[a6+1]*second1)*first2 + (src_y0[a6+src_stride]*second2 + src_y0[a6+src_stride+1]*second1)*first1)>>20;
			a5 = ((src_y1[a6]*second2 + src_y1[a6+1]*second1)*second2 + (src_y1[a6+src_stride]*second2 + src_y1[a6+src_stride+1]*second1)*second1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	

			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);

			x_scale_par0 += 18;			
			*((VO_U32*)dst) = a4;
			dst += dst_stride;

		}while((i-=6) != 0);

		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;

	}while((height-=6) != 0);
}

void cc_yuv420_mb_s_l90_c_double(VO_S32 width, VO_S32 height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
								 VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))<<16)|a4;

			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////

			a4 = (src_y0[a6] + src_y0[a6+1])>>1;
			a5 = (src_y1[a6] + src_y1[a6+1] + src_y1[a6+src_stride] + src_y1[a6+src_stride+1])>>2;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	
			a4 = ((((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))<<16)|a4;


			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			x_scale_par0 += 6;

		}while((i-=2) != 0);

		dst += (width*dst_stride);
		dst += 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}
void cc_yuv420_mb_s_r90_c_double(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
								 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////

			a4 = (src_y0[a6] + src_y0[a6+1])>>1;
			a5 = (src_y1[a6] + src_y1[a6+1] + src_y1[a6+src_stride] + src_y1[a6+src_stride+1])>>2;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHERNEW_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHERNEW_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			x_scale_par0 += 6;

		}while((i-=2) != 0);

		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;
	}while((height-=2) != 0);
}

void cc_yuv420_rgb32_mb_rotation_90r_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a2 -= 128;
			a3 -= 128;

			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip255[(((a4 + a0)>>20) + RDITHERNEW_P0)])<<16)|((ccClip255[(((a4 - a1)>>20) + GDITHER_P0)])<<8)|(ccClip255[(((a4 + a2)>>20) + BDITHERNEW_P0)]) | ( alpha_value << 24 );	
			a5 = (((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P1)])<<16)|((ccClip255[(((a5 - a1)>>20) + GDITHER_P1)])<<8)|(ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P1)])) | ( alpha_value << 24 );

			*((VO_U32*)(dst+4)) = a4;
			*((VO_U32*)dst)     = a5;
			dst += dst_stride;

			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip255[(((a4 + a0)>>20) + RDITHERNEW_P2)])<<16)|((ccClip255[(((a4 - a1)>>20) + GDITHER_P2)])<<8)|(ccClip255[(((a4 + a2)>>20) + BDITHERNEW_P2)]) | ( alpha_value << 24 );	
			a5 = (((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P3)])<<16)|((ccClip255[(((a5 - a1)>>20) + GDITHER_P3)])<<8)|(ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P3)])) | ( alpha_value << 24 );


			*((VO_U32*)(dst+4)) = a4;
			*((VO_U32*)dst)     = a5;
			dst += dst_stride;

		}while((i-=2) != 0);

		dst -= (width*dst_stride);
		dst -= 8;
		y_scale_par += 6;
	}while((height-=2) != 0);

}

void cc_yuv420_rgb32_mb_rotation_90r_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
										      VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];
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

			a4 = ((ccClip255[(a4 + a0)>>20])<<16)|((ccClip255[(a4 - a1)>>20])<<8)|(ccClip255[(a4 + a2)>>20]) | ( alpha_value << 24 );	
			a5 = (((ccClip255[(a5 + a0)>>20])<<16)|((ccClip255[(a5 - a1)>>20])<<8)|(ccClip255[(a5 + a2)>>20])) | ( alpha_value << 24 );

			*((VO_U32*)(dst+4)) = a4;
			*((VO_U32*)dst)     = a5;
			dst += dst_stride;

			x_scale_par0 += 3;
			a4 = src_y0[a6];
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip255[(a4 + a0)>>20])<<16)|((ccClip255[(a4 - a1)>>20])<<8)|(ccClip255[(a4 + a2)>>20]) | ( alpha_value << 24 );	
			a5 = (((ccClip255[(a5 + a0)>>20])<<16)|((ccClip255[(a5 - a1)>>20])<<8)|(ccClip255[(a5 + a2)>>20])) | ( alpha_value << 24 );


			*((VO_U32*)(dst+4)) = a4;
			*((VO_U32*)dst)     = a5;
			dst += dst_stride;

		}while((i-=2) != 0);

		dst -= (width*dst_stride);
		dst -= 8;
		y_scale_par += 6;
	}while((height-=2) != 0);

}

void cc_yuv420_argb32_mb_rotation_90r_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
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

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a2 -= 128;
			a3 -= 128;

			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip255[(((a4 + a0)>>20) + RDITHERNEW_P0)]))|((ccClip255[(((a4 - a1)>>20) + GDITHER_P0)])<<8)|((ccClip255[(((a4 + a2)>>20) + BDITHERNEW_P0)])<<16)|(255 << 24);	
			a5 = ((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P1)]))|((ccClip255[(((a5 - a1)>>20) + GDITHER_P1)])<<8)|((ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P1)])<<16)|(255 << 24);

			*((VO_U32*)(dst+4)) = a4;
			*((VO_U32*)dst)     = a5;
			dst += dst_stride;

			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip255[(((a4 + a0)>>20) + RDITHERNEW_P2)]))|((ccClip255[(((a4 - a1)>>20) + GDITHER_P2)])<<8)|((ccClip255[(((a4 + a2)>>20) + BDITHERNEW_P2)])<<16)|(255 << 24);	
			a5 = ((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P3)]))|((ccClip255[(((a5 - a1)>>20) + GDITHER_P3)])<<8)|((ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P3)])<<16)|(255 << 24);


			*((VO_U32*)(dst+4)) = a4;
			*((VO_U32*)dst)     = a5;
			dst += dst_stride;

		}while((i-=2) != 0);

		dst -= (width*dst_stride);
		dst -= 8;
		y_scale_par += 6;
	}while((height-=2) != 0);

}

void cc_yuv420_argb32_mb_rotation_90r_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
										       VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;

	do{
		VO_S32 *x_scale_par0 = x_scale_par;
		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;

		//VO_S32 aa1 = y_scale_par[4];
		//VO_S32 bb1 = y_scale_par[5];	

		i = width;		
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6;
			// u, v
			a0 = x_scale_par0[0];//src pos0
			//			a1 = ((a0+1)>>1);

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];
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

			a4 = ((ccClip255[(a4 + a0)>>20]))|((ccClip255[(a4 - a1)>>20])<<8)|((ccClip255[(a4 + a2)>>20])<<16)|(255 << 24);	
			a5 = ((ccClip255[(a5 + a0)>>20]))|((ccClip255[(a5 - a1)>>20])<<8)|((ccClip255[(a5 + a2)>>20])<<16)|(255 << 24);

			*((VO_U32*)(dst+4)) = a4;
			*((VO_U32*)dst)     = a5;
			dst += dst_stride;

			x_scale_par0 += 3;
			a4 = src_y0[a6];
			a5 = src_y1[a6];

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip255[(a4 + a0)>>20]))|((ccClip255[(a4 - a1)>>20])<<8)|((ccClip255[(a4 + a2)>>20])<<16)|(255 << 24);	
			a5 = ((ccClip255[(a5 + a0)>>20]))|((ccClip255[(a5 - a1)>>20])<<8)|((ccClip255[(a5 + a2)>>20])<<16)|(255 << 24);

			*((VO_U32*)(dst+4)) = a4;
			*((VO_U32*)dst)     = a5;
			dst += dst_stride;

		}while((i-=2) != 0);

		dst -= (width*dst_stride);
		dst -= 8;
		y_scale_par += 6;
	}while((height-=2) != 0);

}

void cc_yuv420_mb_rotation_90r_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
								   VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, 
								   const VO_S32 vin_stride)
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

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a2 -= 128;
			a3 -= 128;

			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1))|(a4<<16);

			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			//a6 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a4 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);	
			a4 = (((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P3)>>2]>>1))|(a4<<16);


			*((VO_U32*)dst) = a4;
			dst += dst_stride;

		}while((i-=2) != 0);

		dst -= (width*dst_stride);
		dst -= 4;
		y_scale_par += 6;
	}while((height-=2) != 0);

}

void cc_yuv420_rgb32_mb_rotation_180_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
								         VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, 
								         const VO_S32 vin_stride)
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

		dst = dst + (width<<2) - 8;
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

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a2 -= 128;
			a3 -= 128;

			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip255[(((a4 + a0)>>20) + RDITHERNEW_P0)])<<16)|((ccClip255[(((a4 - a1)>>20) + GDITHER_P0)])<<8)|(ccClip255[(((a4 + a2)>>20) + BDITHERNEW_P0)]);	
			a7 = ((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P1)])<<16)|((ccClip255[(((a5 - a1)>>20) + GDITHER_P1)])<<8)|(ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P1)]);

			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P2)])<<16)|((ccClip255[(((a5 - a1)>>20) + GDITHER_P2)])<<8)|(ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P2)]);//|a4;
			a8 = ((ccClip255[(((a8 + a0)>>20) + RDITHERNEW_P3)])<<16)|((ccClip255[(((a8 - a1)>>20) + GDITHER_P3)])<<8)|(ccClip255[(((a8 + a2)>>20) + BDITHERNEW_P3)]);//|a7;

			*((VO_U32*)(dst+4)) = a4;
			*((VO_U32*)(dst)) = a5;
			*((VO_U32*)(dst-dst_stride+4))   = a7;
			*((VO_U32*)(dst-dst_stride)) = a8;
			dst -= 8;
		}while((i-=2) != 0);

		dst += 8;
		dst -= (dst_stride<<1);
		y_scale_par += 6;
	}while((height-=2) != 0);

}

void cc_yuv420_argb32_mb_rotation_180_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, 
										  const VO_S32 vin_stride)
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

		dst = dst + (width<<2) - 8;
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

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a2 -= 128;
			a3 -= 128;

			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip255[(((a4 + a0)>>20) + RDITHERNEW_P0)]))|((ccClip255[(((a4 - a1)>>20) + GDITHER_P0)])<<8)|((ccClip255[(((a4 + a2)>>20) + BDITHERNEW_P0)])<<16)|(255 << 24);	
			a7 = ((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P1)]))|((ccClip255[(((a5 - a1)>>20) + GDITHER_P1)])<<8)|((ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P1)])<<16)|(255 << 24);

			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip255[(((a5 + a0)>>20) + RDITHERNEW_P2)]))|((ccClip255[(((a5 - a1)>>20) + GDITHER_P2)])<<8)|((ccClip255[(((a5 + a2)>>20) + BDITHERNEW_P2)])<<16)|(255 << 24);//|a4;
			a8 = ((ccClip255[(((a8 + a0)>>20) + RDITHERNEW_P3)]))|((ccClip255[(((a8 - a1)>>20) + GDITHER_P3)])<<8)|((ccClip255[(((a8 + a2)>>20) + BDITHERNEW_P3)])<<16)|(255 << 24);//|a7;

			*((VO_U32*)(dst+4)) = a4;
			*((VO_U32*)(dst)) = a5;
			*((VO_U32*)(dst-dst_stride+4))   = a7;
			*((VO_U32*)(dst-dst_stride)) = a8;
			dst -= 8;
		}while((i-=2) != 0);

		dst += 8;
		dst -= (dst_stride<<1);
		y_scale_par += 6;
	}while((height-=2) != 0);

}

void cc_yuv420_mb_rotation_180_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
								   VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, 
								   const VO_S32 vin_stride)
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

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			a2 -= 128;
			a3 -= 128;

			a0 = (a3 * ConstV1);
			a1 = (a3 * ConstV2 + a2 *ConstU2);
			a2 = (a2 * ConstU1);

			a4 = (a4 - 16) * ConstY;
			a5 = (a5 - 16) * ConstY;

			a4 = ((ccClip63[(((a4 + a0)>>20) + RDITHERNEW_P0)>>2]>>1)<<11)|((ccClip63[(((a4 - a1)>>20) + GDITHER_P0)>>2])<<5)|(ccClip63[(((a4 + a2)>>20) + BDITHERNEW_P0)>>2]>>1);	
			a7 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P1)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P1)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P1)>>2]>>1);

			/////////////////////////////////////////////////////////////////////
			//			a6 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			a5 = (a5 - 16) * ConstY;
			a8 = (a8 - 16) * ConstY;

			a5 = ((ccClip63[(((a5 + a0)>>20) + RDITHERNEW_P2)>>2]>>1)<<11)|((ccClip63[(((a5 - a1)>>20) + GDITHER_P2)>>2])<<5)|(ccClip63[(((a5 + a2)>>20) + BDITHERNEW_P2)>>2]>>1);//|a4;
			a8 = ((ccClip63[(((a8 + a0)>>20) + RDITHERNEW_P3)>>2]>>1)<<11)|((ccClip63[(((a8 - a1)>>20) + GDITHER_P3)>>2])<<5)|(ccClip63[(((a8 + a2)>>20) + BDITHERNEW_P3)>>2]>>1);//|a7;

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
		dst -= (dst_stride<<1);
		y_scale_par += 6;
	}while((height-=2) != 0);

}

#ifdef COLCONVENABLE

#undef WRITE_PIX

#define WRITE_PIX(a4, x){\
	*((VO_S32*)(out_buf+(x))) = (VO_S32)a4;\
	out_buf += (x);\
}

void YUV420PlanarToRGB16_MB_rotation_90L(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride, VO_U8 *out_buf,
										 const VO_S32 out_stride, const VO_S32* const param_tab, const VO_U32 is420planar, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5,a7;
		//0
		LOAD_DATA(a3, a7, 0, 0)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 1)
		PIX_1(a7)

		WRITE_PIX(a4, 0)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+1)
		PIX_3(a7)

		WRITE_PIX(a4, -out_stride)

		//1	
		LOAD_DATA(a3, a7, 1, 2)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 3)	
		PIX_1(a7)

		WRITE_PIX(a4, -out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+3)
		PIX_3(a7)

		WRITE_PIX(a4, -out_stride)

		//2
		LOAD_DATA(a3, a7, 2, 4)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 5)
		PIX_1(a7)

		WRITE_PIX(a4, -out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+5)
		PIX_3(a7)

		WRITE_PIX(a4, -out_stride)

		//3
		LOAD_DATA(a3, a7, 3, 6)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 7)	
		PIX_1(a7)

		WRITE_PIX(a4, -out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+7)
		PIX_3(a7)

		WRITE_PIX(a4, -out_stride)

		//4
		LOAD_DATA(a3, a7, 4, 8)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 9)	
		PIX_1(a7)

		WRITE_PIX(a4, -out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+9)
		PIX_3(a7)

		WRITE_PIX(a4, -out_stride)

		//5
		LOAD_DATA(a3, a7, 5, 10)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 11)	
		PIX_1(a7)

		WRITE_PIX(a4, -out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+11)
		PIX_3(a7)

		WRITE_PIX(a4, -out_stride)

		//6
		LOAD_DATA(a3, a7, 6, 12)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 13)	
		PIX_1(a7)

		WRITE_PIX(a4, -out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+13)
		PIX_3(a7)

		WRITE_PIX(a4, -out_stride)

		//7
		LOAD_DATA(a3, a7, 7, 14)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 15)	
		PIX_1(a7)

		WRITE_PIX(a4, -out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+15)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		psrc_y += (in_stride<<1);
		psrc_u += (in_stride>>is420planar);
		psrc_v += (in_stride>>is420planar);
		start_buf += 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX1(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX3(a1, a3)\
	a1 -= 128;\
}
void YUYVToRGB16_MB_rotation_90L(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX0(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX2(a3, a3)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX0(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a7, a7)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX0(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a8, a8)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX0(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a3, a3)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX0(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a7, a7)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX0(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a8, a8)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX0(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a3, a3)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX0(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a7, a7)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)


		psrc += (in_stride<<1);
		start_buf += 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

void YUYV2ToRGB16_MB_rotation_90L(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								  const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX2(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX0(a3, a3)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX2(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a7, a7)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX2(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a8, a8)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX2(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a3, a3)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX2(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a7, a7)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX2(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a8, a8)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX2(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a3, a3)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX2(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a7, a7)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)


		psrc += (in_stride<<1);
		start_buf += 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX3(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX1(a1, a3)\
	a1 -= 128;\
}
void YVYUToRGB16_MB_rotation_90L(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride, 
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX0(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX2(a3, a3)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX0(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a7, a7)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX0(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a8, a8)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX0(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a3, a3)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX0(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a7, a7)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX0(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a8, a8)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX0(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a3, a3)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX0(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX2(a7, a7)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)


		psrc += (in_stride<<1);
		start_buf += 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

void YVYU2ToRGB16_MB_rotation_90L(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride, 
								  const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX2(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX0(a3, a3)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX2(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a7, a7)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX2(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a8, a8)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX2(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a3, a3)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX2(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a7, a7)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX2(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a8, a8)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX2(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a3, a3)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX2(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX0(a7, a7)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)


		psrc += (in_stride<<1);
		start_buf += 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX0(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX2(a1, a3)\
	a1 -= 128;\
}

void UYVYToRGB16_MB_rotation_90L(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX1(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX3(a3, a3)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX1(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a7, a7)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX1(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a8, a8)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX1(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a3, a3)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX1(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a7, a7)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX1(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a8, a8)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX1(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a3, a3)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX1(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a7, a7)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)


		psrc += (in_stride<<1);
		start_buf += 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

void UYVY2ToRGB16_MB_rotation_90L(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride, 
								  const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX3(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX1(a3, a3)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX3(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a7, a7)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX3(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a8, a8)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX3(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a3, a3)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX3(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a7, a7)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX3(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a8, a8)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX3(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a3, a3)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX3(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a7, a7)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)


		psrc += (in_stride<<1);
		start_buf += 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX2(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX0(a1, a3)\
	a1 -= 128;\
}

void VYUYToRGB16_MB_rotation_90L(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX1(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX3(a3, a3)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX1(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a7, a7)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX1(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a8, a8)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX1(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a3, a3)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX1(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a7, a7)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX1(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a8, a8)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX1(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a3, a3)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX1(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX3(a7, a7)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)


		psrc += (in_stride<<1);
		start_buf += 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

void VYUY2ToRGB16_MB_rotation_90L(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								  const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX3(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX1(a3, a3)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX3(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a7, a7)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX3(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a8, a8)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX3(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a3, a3)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX3(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a7, a7)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX3(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a8, a8)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, -out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX3(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a3, a3)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, -out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX3(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, -out_stride)


		GET_PIX1(a7, a7)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, -out_stride)


		psrc += (in_stride<<1);
		start_buf += 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

///////////////////////////////////////////////////////////////////////Rotation right 90
#undef PIX_0
#undef PIX_1
#undef PIX_2
#undef PIX_3

#define PIX_0(a3){\
	a4  = ccClip31[(a3 + a1 + RBDITHER_P2)>>3];\
	a5  = ccClip63[(a3 - a0 + GDITHER_P0)>>2];\
	a3  = ccClip31[(a3 + a2 + RBDITHER_P0)>>3];\
	a4 = ((((a4<<6)|a5 )<<21))|(a3<<16);\
}
#define PIX_1(a3){\
	a5  = ccClip31[(a3 + a2 + RBDITHER_P1)>>3];\
	a4 |= a5;\
	a5  = ccClip31[(a3 + a1 + RBDITHER_P3)>>3];\
	a3  = ccClip63[(a3 - a0 + GDITHER_P1)>>2];\
	a3 |= (a5<<6);\
	a4 |= (a3<<5);\
}

#define PIX_2(a3){\
	a4  = ccClip31[(a3 + a1 + RBDITHER_P0)>>3];\
	a5  = ccClip63[(a3 - a0 + GDITHER_P2)>>2];\
	a3  = ccClip31[(a3 + a2 + RBDITHER_P2)>>3];\
	a4 = ((((a4<<6)|a5 )<<21))|(a3<<16);\
}
#define PIX_3(a3){\
	a5  = ccClip31[(a3 + a2 + RBDITHER_P3)>>3];\
	a4 |= a5;\
	a5  = ccClip31[(a3 + a1 + RBDITHER_P1)>>3];\
	a3  = ccClip63[(a3 - a0 + GDITHER_P3)>>2];\
	a3 |= (a5<<6);\
	a4 |= (a3<<5);\
}

void YUV420PlanarToRGB16_MB_rotation_90R(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride, VO_U8 *out_buf, 
										 const VO_S32 out_stride, const VO_S32* const param_tab, const VO_U32 is420planar, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5,a7;
		//0
		LOAD_DATA(a3, a7, 0, 0)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 1)
		PIX_1(a7)

		WRITE_PIX(a4, 0)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+1)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride)

		//1	
		LOAD_DATA(a3, a7, 1, 2)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 3)	
		PIX_1(a7)

		WRITE_PIX(a4, out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+3)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride)

		//2
		LOAD_DATA(a3, a7, 2, 4)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 5)
		PIX_1(a7)

		WRITE_PIX(a4, out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+5)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride)

		//3
		LOAD_DATA(a3, a7, 3, 6)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 7)	
		PIX_1(a7)

		WRITE_PIX(a4, out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+7)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride)

		//4
		LOAD_DATA(a3, a7, 4, 8)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 9)	
		PIX_1(a7)

		WRITE_PIX(a4, out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+9)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride)

		//5
		LOAD_DATA(a3, a7, 5, 10)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 11)	
		PIX_1(a7)

		WRITE_PIX(a4, out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+11)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride)

		//6
		LOAD_DATA(a3, a7, 6, 12)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 13)	
		PIX_1(a7)

		WRITE_PIX(a4, out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+13)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride)

		//7
		LOAD_DATA(a3, a7, 7, 14)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 15)	
		PIX_1(a7)

		WRITE_PIX(a4, out_stride)

		PIX_2(a3)
		LOAD_YPIX(a7, in_stride+15)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		psrc_y += (in_stride<<1);
		psrc_u += (in_stride>>is420planar);
		psrc_v += (in_stride>>is420planar);
		start_buf -= 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX1(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX3(a1, a3)\
	a1 -= 128;\
}
void YUYVToRGB16_MB_rotation_90R(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX0(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX2(a3, a3)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX0(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a7, a7)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX0(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a8, a8)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX0(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a3, a3)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX0(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a7, a7)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX0(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a8, a8)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX0(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a3, a3)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX0(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a7, a7)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)


		psrc += (in_stride<<1);
		start_buf -= 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

void YUYV2ToRGB16_MB_rotation_90R(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								  const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX2(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX0(a3, a3)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX2(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a7, a7)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX2(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a8, a8)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX2(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a3, a3)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX2(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a7, a7)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX2(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a8, a8)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX2(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a3, a3)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX2(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a7, a7)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)


		psrc += (in_stride<<1);
		start_buf -= 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}
#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX3(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX1(a1, a3)\
	a1 -= 128;\
}
void YVYUToRGB16_MB_rotation_90R(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride, 
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX0(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX2(a3, a3)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX0(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a7, a7)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX0(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a8, a8)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX0(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a3, a3)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX0(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a7, a7)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX0(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a8, a8)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX0(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a3, a3)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX0(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, out_stride)


		GET_PIX2(a7, a7)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)


		psrc += (in_stride<<1);
		start_buf -= 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

void YVYU2ToRGB16_MB_rotation_90R(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								  const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX2(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX0(a3, a3)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX2(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a7, a7)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX2(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a8, a8)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX2(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a3, a3)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX2(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a7, a7)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX2(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a8, a8)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX2(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a3, a3)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX2(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, out_stride)


		GET_PIX0(a7, a7)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)


		psrc += (in_stride<<1);
		start_buf -= 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX0(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX2(a1, a3)\
	a1 -= 128;\
}
void UYVYToRGB16_MB_rotation_90R(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX1(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX3(a3, a3)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX1(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a7, a7)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX1(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a8, a8)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX1(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a3, a3)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX1(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a7, a7)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX1(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a8, a8)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX1(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a3, a3)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX1(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a7, a7)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)


		psrc += (in_stride<<1);
		start_buf -= 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}
void UYVY2ToRGB16_MB_rotation_90R(VO_U8 *psrc, const VO_S32 in_stride,VO_U8 *out_buf, const VO_S32 out_stride, 
								  const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX3(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX1(a3, a3)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX3(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a7, a7)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX3(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a8, a8)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX3(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a3, a3)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX3(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a7, a7)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX3(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a8, a8)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX3(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a3, a3)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX3(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a7, a7)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)


		psrc += (in_stride<<1);
		start_buf -= 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}
#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX2(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX0(a1, a3)\
	a1 -= 128;\
}
void VYUYToRGB16_MB_rotation_90R(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX1(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX3(a3, a3)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX1(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a7, a7)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX1(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a8, a8)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX1(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a3, a3)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX1(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a7, a7)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX1(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a8, a8)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX1(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a3, a3)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX1(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, out_stride)


		GET_PIX3(a7, a7)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)


		psrc += (in_stride<<1);
		start_buf -= 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

void VYUY2ToRGB16_MB_rotation_90R(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								  const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	VO_U8 *start_buf = out_buf;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX3(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 4)
		WRITE_PIX(a4, 0)

		GET_PIX1(a3, a3)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX3(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 8)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a7, a7)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX3(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 12)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a8, a8)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX3(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 16)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a3, a3)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX3(a8, a3)	//Y2
		PIX_1(a8)
		LOAD_4BYTE(a8, 20)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a7, a7)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		PIX_0(a3)
		GET_PIX3(a3, a7)	//Y2
		PIX_1(a3)
		LOAD_4BYTE(a3, 24)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a8, a8)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		PIX_0(a7)
		GET_PIX3(a7, a8)	//Y2
		PIX_1(a7)
		LOAD_4BYTE(a7, 28)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a3, a3)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		PIX_0(a8)
		GET_PIX3(a8, a3)	//Y2
		PIX_1(a8)
		WRITE_PIX(a4, out_stride)


		GET_PIX1(a7, a7)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride)


		psrc += (in_stride<<1);
		start_buf -= 4;
		out_buf = start_buf;

	}while(--a6 != 0);
}

///////////////////////////////////////////////////////////////////////Rotation right 180
#undef WRITE_PIX
#define WRITE_PIX(a4, x){\
	*((VO_S32*)(out_buf-(x))) = (VO_S32)a4;\
}
void YUV420PlanarToRGB16_MB_rotation_180(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride, VO_U8 *out_buf,const VO_S32 out_stride,
										 const VO_S32* const param_tab, const VO_U32 is420planar,  const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5,a7;
		//0
		LOAD_DATA(a3, a7, 0, 0)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 1)
		PIX_1(a3)

		WRITE_PIX(a4, 0)
		PIX_2(a7)
		LOAD_YPIX(a7, in_stride+1)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride)

		//1	
		LOAD_DATA(a3, a7, 1, 2)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 3)	
		PIX_1(a3)

		WRITE_PIX(a4, 4)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride+3)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride + 4)

		//2
		LOAD_DATA(a3, a7, 2, 4)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 5)
		PIX_1(a3)

		WRITE_PIX(a4, 8)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride+5)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride+8)

		//3
		LOAD_DATA(a3, a7, 3, 6)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 7)	
		PIX_1(a3)

		WRITE_PIX(a4, 12)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride+7)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride+12)

		//4
		LOAD_DATA(a3, a7, 4, 8)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 9)	
		PIX_1(a3)

		WRITE_PIX(a4, 16)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride+9)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride+16)

		//5
		LOAD_DATA(a3, a7, 5, 10)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 11)	
		PIX_1(a3)

		WRITE_PIX(a4, 20)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride+11)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride+20)

		//6
		LOAD_DATA(a3, a7, 6, 12)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 13)	
		PIX_1(a3)

		WRITE_PIX(a4, 24)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride+13)
		PIX_3(a7)

		WRITE_PIX(a4, out_stride+24)

		//7
		LOAD_DATA(a3, a7, 7, 14)
		CAL_UV()
		PIX_0(a3)
		LOAD_YPIX(a3, 15)	
		PIX_1(a3)

		WRITE_PIX(a4, 28)

		PIX_2(a7)
		LOAD_YPIX(a7, in_stride+15)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride+28)

		psrc_y += (in_stride<<1);
		psrc_u += (in_stride>>is420planar);
		psrc_v += (in_stride>>is420planar);
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}


#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX1(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX3(a1, a3)\
	a1 -= 128;\
}
void YUYVToRGB16_MB_rotation_180(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride, 
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}

void YUYV2ToRGB16_MB_rotation_180(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								  const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}

#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX3(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX1(a1, a3)\
	a1 -= 128;\
}
void YVYUToRGB16_MB_rotation_180(VO_U8 *psrc, const VO_S32 in_stride,VO_U8 *out_buf, const VO_S32 out_stride, 
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX0(a3, a8)	//Y1
		GET_PIX2(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX0(a8, a7)	//Y1
		GET_PIX2(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX0(a7, a3)	//Y1
		GET_PIX2(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}

void YVYU2ToRGB16_MB_rotation_180(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								  const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX2(a3, a8)	//Y1
		GET_PIX0(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX2(a8, a7)	//Y1
		GET_PIX0(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX2(a7, a3)	//Y1
		GET_PIX0(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}

#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX0(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX2(a1, a3)\
	a1 -= 128;\
}
void UYVYToRGB16_MB_rotation_180(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}

void UYVY2ToRGB16_MB_rotation_180(VO_U8 *psrc, const VO_S32 in_stride,
								  VO_U8 *out_buf, const VO_S32 out_stride, const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}

#undef GET_U
#undef GET_V

#define GET_U(a0, a3){\
	GET_PIX2(a0, a3)\
	a0 -= 128;\
}
#define GET_V(a1, a3){\
	GET_PIX0(a1, a3)\
	a1 -= 128;\
}
void VYUYToRGB16_MB_rotation_180(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride, 
								 const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX1(a3, a8)	//Y1
		GET_PIX3(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX1(a8, a7)	//Y1
		GET_PIX3(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX1(a7, a3)	//Y1
		GET_PIX3(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}

void VYUY2ToRGB16_MB_rotation_180(VO_U8 *psrc, const VO_S32 in_stride, VO_U8 *out_buf, const VO_S32 out_stride,
								  const VO_S32* const param_tab, const VO_U32 isRGB565)
{
	VO_U32 a6 = 8;

	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;

		//0
		LOAD_4BYTE(a3, 0)
		LOAD_4BYTE(a8, in_stride)
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 4)
		PIX_1(a3)
		WRITE_PIX(a4, 0)


		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 4)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride)

		//1
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 8)
		PIX_1(a7)
		WRITE_PIX(a4, 4)


		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 8)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 4)

		//2
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 12)
		PIX_1(a8)
		WRITE_PIX(a4, 8)


		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 12)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 8)

		//3
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 16)
		PIX_1(a3)
		WRITE_PIX(a4, 12)


		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 16)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 12)

		//4
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_0(a8)
		LOAD_4BYTE(a8, 20)
		PIX_1(a7)
		WRITE_PIX(a4, 16)


		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		LOAD_4BYTE(a7, in_stride + 20)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 16)

		//5
		GET_U(a0, a8)		//U
		GET_V(a1, a8)		//V
		CAL_UV()
		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_0(a3)
		LOAD_4BYTE(a3, 24)
		PIX_1(a8)
		WRITE_PIX(a4, 20)


		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_2(a8)
		LOAD_4BYTE(a8, in_stride + 24)
		PIX_3(a7)
		WRITE_PIX(a4, out_stride + 20)

		//6
		GET_U(a0, a3)		//U
		GET_V(a1, a3)		//V
		CAL_UV()
		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_0(a7)
		LOAD_4BYTE(a7, 28)
		PIX_1(a3)
		WRITE_PIX(a4, 24)


		GET_PIX3(a3, a8)	//Y1
		GET_PIX1(a8, a8)	//Y2
		PIX_2(a3)
		LOAD_4BYTE(a3, in_stride + 28)
		PIX_3(a8)
		WRITE_PIX(a4, out_stride + 24)

		//7
		GET_U(a0, a7)		//U
		GET_V(a1, a7)		//V
		CAL_UV()
		GET_PIX3(a8, a7)	//Y1
		GET_PIX1(a7, a7)	//Y2
		PIX_0(a8)
		PIX_1(a7)
		WRITE_PIX(a4, 28)


		GET_PIX3(a7, a3)	//Y1
		GET_PIX1(a3, a3)	//Y2
		PIX_2(a7)
		PIX_3(a3)
		WRITE_PIX(a4, out_stride + 28)


		psrc += (in_stride<<1);
		out_buf -= (out_stride<<1);

	}while(--a6 != 0);
}
#endif //COLCONVENABLE

#ifdef VOX86
static  __m128i muly(const __m128i a, const __m128i b)
{
    __m128i tmp1 = _mm_mul_epu32(a,b); /* mul 2,0*/
    __m128i tmp2 = _mm_mul_epu32( _mm_srli_si128(a,4), _mm_srli_si128(b,4)); /* mul 3,1 */
    return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE (0,0,2,0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE (0,0,2,0))); /* shuffle results to [63..0] and pack */
}



void cc_yuv2yuv_16x16_s_x86(VO_S32 width, VO_S32 height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst,VO_S32 src_stride,VO_S32 dst_stride,
							 VO_S32 *x_scale_par, VO_S32 *y_scale_par, VO_S32 uin_stride,VO_S32 vin_stride,VO_U8 *dsty,VO_U8 *dstu, VO_U8 *dstv)
{
#if 0
	VO_S32 i, j;
    __m128i  m_y0, m_y1, m_xbbaa, m_ybbaa;

	for(j = 0; j < height; j+= 2)
	{
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;

		VO_S32 aa1 = y_scale_par[4];
		VO_S32 bb1 = y_scale_par[5];	

		for(i = 0; i< width; i+= 2)		
		{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			VO_S32 aa, bb;
			VO_S32 aa1, bb1;
			VO_U32 result;

			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];

			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

//			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
//			a5 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			aa1 = x_scale_par0[1];
			bb1 = x_scale_par0[2];

			x_scale_par0 += 3;
//			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
//			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			m_xbbaa = _mm_set_epi16(bb, aa, bb, aa, bb1, aa1, bb1, aa1);
			m_ybbaa = _mm_set_epi16(bb, aa, bb, aa, bb1, aa1, bb1, aa1);

//			m_y0 = _mm_set_epi16(src_y0[a0], src_y0[a0+1], src_y1[a0], src_y1[a0+1],
//				                 src_y0[a6], src_y0[a6+1], src_y1[a6], src_y1[a6+1]);

			a0 = a0+src_stride;
			a6 = a6+src_stride;

//			m_y1 =  _mm_set_epi16(src_y0[a0], src_y0[a0+1], src_y1[a0], src_y1[a0+1],
//				                  src_y0[a6], src_y0[a6+1], src_y1[a6], src_y1[a6+1]);

			m_y0 = _mm_madd_epi16(m_y0, m_xbbaa);
			m_y1 = _mm_madd_epi16(m_y1, m_xbbaa);

			m_y0 = muly(m_y0, m_ybbaa);
			m_y1 = muly(m_y1, m_ybbaa);

			m_y0 = _mm_add_epi32(m_y0, m_y1);
            m_y0 = _mm_srai_epi32(m_y0, 20);

			m_y0 = _mm_packs_epi32(m_y0, m_y0);
			m_y0 = _mm_packs_epi16(m_y0, m_y0);

			result = _mm_cvtsi128_si32(m_y0);

			*((VO_U16*)dsty) = result & 0xFFFF;
			*((VO_U16*)(dsty + dst_stride)) = (result >> 16) & 0xFFFF;

			dstu[0] = (VO_U8)a2;
            dstv[0] = (VO_U8)a3;

			dsty += 2;
			dstu++;
			dstv++;
		}

		dsty -= width;
		dstu -= width / 2;
		dstv -= width / 2;

		dsty += (dst_stride<<1);
        dstu +=  dst_stride / 2;
		dstv +=  dst_stride / 2;

		y_scale_par += 6;
	 }
#else
	VO_S32 i, j;

	for(j = 0; j < height; j+= 2)
	{
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;

		VO_S32 aa1 = y_scale_par[4];
		VO_S32 bb1 = y_scale_par[5];	

		for(i = 0; i< width; i+= 2)		
		{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			VO_S32 aa, bb;

			a0 = x_scale_par0[0];//src pos0
			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];

			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = ((src_y0[a0]*bb + src_y0[a0+1]*aa)*bb0 + (src_y0[a0+src_stride]*bb + src_y0[a0+src_stride+1]*aa)*aa0)>>20;
			a7 = ((src_y1[a0]*bb + src_y1[a0+1]*aa)*bb1 + (src_y1[a0+src_stride]*bb + src_y1[a0+src_stride+1]*aa)*aa1)>>20;

			aa = x_scale_par0[1];
			bb = x_scale_par0[2];

			x_scale_par0 += 3;
			a5 = ((src_y0[a6]*bb + src_y0[a6+1]*aa)*bb0 + (src_y0[a6+src_stride]*bb + src_y0[a6+src_stride+1]*aa)*aa0)>>20;
			a8 = ((src_y1[a6]*bb + src_y1[a6+1]*aa)*bb1 + (src_y1[a6+src_stride]*bb + src_y1[a6+src_stride+1]*aa)*aa1)>>20;

			dsty[0] = (VO_U8)a4;
			dsty[1] = (VO_U8)a5;
			dsty[0 + dst_stride] = (VO_U8)a7;
			dsty[1 + dst_stride] = (VO_U8)a8;

			dstu[0] = (VO_U8)a2;
            dstv[0] = (VO_U8)a3;

			dsty += 2;
			dstu++;
			dstv++;
		}

		dsty -= width;
		dstu -= width / 2;
		dstv -= width / 2;

		dsty += (dst_stride<<1);
        dstu +=  dst_stride / 2;
		dstv +=  dst_stride / 2;

		y_scale_par += 6;
	 }
#endif	
}



void cc_yuv2yuv_16x16_s_x86_half(VO_S32 width, VO_S32 height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst,VO_S32 src_stride,VO_S32 dst_stride,
							 VO_S32 *x_scale_par, VO_S32 *y_scale_par, VO_S32 uin_stride,VO_S32 vin_stride,VO_U8 *dsty,VO_U8 *dstu, VO_U8 *dstv)
{
	VO_S32 i, j;

	for(j = 0; j < height; j+= 2)
	{
		VO_S32 *x_scale_par0 = x_scale_par;

		VO_U8 *src_y0 = src_y + y_scale_par[0] * src_stride; // TBD
		VO_U8 *src_u0 = src_u + ((y_scale_par[0]>>1) * uin_stride);
		VO_U8 *src_v0 = src_v + ((y_scale_par[0]>>1) * vin_stride);

		VO_S32 aa0 = y_scale_par[1];
		VO_S32 bb0 = y_scale_par[2];
		VO_U8 *src_y1 = src_y + y_scale_par[3] * src_stride;

		VO_S32 aa1 = y_scale_par[4];
		VO_S32 bb1 = y_scale_par[5];	

		for(i = 0; i< width; i+= 2)		
		{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			VO_S32 aa, bb;

			a0 = x_scale_par0[0];//src pos0

			x_scale_par0 += 3;
			a6 = x_scale_par0[0];

			a1 = (a0 + a6 + 1)>>2;

			a2 = src_u0[a1];
			a3 = src_v0[a1];

			a4 = src_y0[a0];
			a7 = src_y1[a0];

			x_scale_par0 += 3;

			a5 = src_y0[a6];
			a8 = src_y1[a6];

			dsty[0] = (VO_U8)a4;
			dsty[1] = (VO_U8)a5;
			dsty[0 + dst_stride] = (VO_U8)a7;
			dsty[1 + dst_stride] = (VO_U8)a8;

			dstu[0] = (VO_U8)a2;
            dstv[0] = (VO_U8)a3;

			dsty += 2;
			dstu++;
			dstv++;
		}

		dsty -= width;
		dstu -= width / 2;
		dstv -= width / 2;

		dsty += (dst_stride<<1);
        dstu +=  dst_stride / 2;
		dstv +=  dst_stride / 2;

		y_scale_par += 6;
	 }
}


void voyuv420toargb32_16nx2n_x86( VO_U8 *dsty, VO_U8 *dstu, VO_U8 *dstv, VO_S32 in_stride, VO_S32 uin_stride, int width, int height, VO_U32 *rgb, VO_S32 srgb )
{
    __m128i y0r0, y0r1, u0, v0;
    __m128i y00r0, y01r0, y00r1, y01r1;
    __m128i u00, u01, v00, v01;
    __m128i rv00, rv01, gu00, gu01, gv00, gv01, bu00, bu01;
    __m128i r00, r01, g00, g01, b00, b01;
    __m128i rgb0123, rgb4567, rgb89ab, rgbcdef;
    __m128i gbgb;
    __m128i ysub, uvsub;
    __m128i zero, mask_ff, facy, facrv, facgu, facgv, facbu;
    __m128i *srcy128r0, *srcy128r1;
    __m128i *dstrgb128r0, *dstrgb128r1;
    __m64   *srcu64, *srcv64;
    int x, y;

    ysub  = _mm_set1_epi32( 0x00100010 );
    uvsub = _mm_set1_epi32( 0x00800080 );
    
    facy  = _mm_set1_epi32( 0x004a004a );
    facrv = _mm_set1_epi32( 0x00660066 );
    facgu = _mm_set1_epi32( 0x00190019 );
    facgv = _mm_set1_epi32( 0x00340034 );
    facbu = _mm_set1_epi32( 0x00810081 );
    
    zero  = _mm_set1_epi32( 0x00000000 );
	mask_ff = _mm_set1_epi32( 0xFFFFFFFF);

    for( y = 0; y < height; y += 2 ) {

        srcy128r0 = (__m128i *)(dsty + in_stride*y);
        srcy128r1 = (__m128i *)(dsty + in_stride*y + in_stride);
        srcu64 = (__m64 *)(dstu + uin_stride*(y/2));
        srcv64 = (__m64 *)(dstv + uin_stride*(y/2));

        dstrgb128r0 = (__m128i *)(rgb + srgb*y);
        dstrgb128r1 = (__m128i *)(rgb + srgb*y + srgb);

        for( x = 0; x < width; x += 16 ) {

            u0 = _mm_loadl_epi64( (__m128i *)srcu64 ); srcu64++;
            v0 = _mm_loadl_epi64( (__m128i *)srcv64 ); srcv64++;

            y0r0 = _mm_loadu_si128( srcy128r0++ );
            y0r1 = _mm_loadu_si128( srcy128r1++ );

            // constant y factors
            y00r0 = _mm_mullo_epi16( _mm_sub_epi16( _mm_unpacklo_epi8( y0r0, zero ), ysub ), facy );
            y01r0 = _mm_mullo_epi16( _mm_sub_epi16( _mm_unpackhi_epi8( y0r0, zero ), ysub ), facy );
            y00r1 = _mm_mullo_epi16( _mm_sub_epi16( _mm_unpacklo_epi8( y0r1, zero ), ysub ), facy );
            y01r1 = _mm_mullo_epi16( _mm_sub_epi16( _mm_unpackhi_epi8( y0r1, zero ), ysub ), facy );

            // expand u and v so they're aligned with y values
            u0  = _mm_unpacklo_epi8( u0,  zero );
            u00 = _mm_sub_epi16( _mm_unpacklo_epi16( u0, u0 ), uvsub );
            u01 = _mm_sub_epi16( _mm_unpackhi_epi16( u0, u0 ), uvsub );

            v0  = _mm_unpacklo_epi8( v0,  zero );
            v00 = _mm_sub_epi16( _mm_unpacklo_epi16( v0, v0 ), uvsub );
            v01 = _mm_sub_epi16( _mm_unpackhi_epi16( v0, v0 ), uvsub );

            // common factors on both rows.
            rv00 = _mm_mullo_epi16( facrv, v00 );
            rv01 = _mm_mullo_epi16( facrv, v01 );
            gu00 = _mm_mullo_epi16( facgu, u00 );
            gu01 = _mm_mullo_epi16( facgu, u01 );
            gv00 = _mm_mullo_epi16( facgv, v00 );
            gv01 = _mm_mullo_epi16( facgv, v01 );
            bu00 = _mm_mullo_epi16( facbu, u00 );
            bu01 = _mm_mullo_epi16( facbu, u01 );

            // row 0
            r00 = _mm_srai_epi16( _mm_add_epi16( y00r0, rv00 ), 6 );
            r01 = _mm_srai_epi16( _mm_add_epi16( y01r0, rv01 ), 6 );
            g00 = _mm_srai_epi16( _mm_sub_epi16( _mm_sub_epi16( y00r0, gu00 ), gv00 ), 6 );
            g01 = _mm_srai_epi16( _mm_sub_epi16( _mm_sub_epi16( y01r0, gu01 ), gv01 ), 6 );
            b00 = _mm_srai_epi16( _mm_add_epi16( y00r0, bu00 ), 6 );
            b01 = _mm_srai_epi16( _mm_add_epi16( y01r0, bu01 ), 6 );

            r00 = _mm_packus_epi16( r00, r01 );         // rrrr.. saturated
            g00 = _mm_packus_epi16( g00, g01 );         // gggg.. saturated
            b00 = _mm_packus_epi16( b00, b01 );         // bbbb.. saturated

            r01     = _mm_unpacklo_epi8(  b00,  mask_ff ); // 0r0r..
            gbgb    = _mm_unpacklo_epi8(  r00,  g00 );  // gbgb..
            rgb0123 = _mm_unpacklo_epi16( gbgb, r01 );  // 0rgb0rgb..
            rgb4567 = _mm_unpackhi_epi16( gbgb, r01 );  // 0rgb0rgb..

            r01     = _mm_unpackhi_epi8(  b00,  mask_ff );
            gbgb    = _mm_unpackhi_epi8(  r00,  g00 );
            rgb89ab = _mm_unpacklo_epi16( gbgb, r01 );
            rgbcdef = _mm_unpackhi_epi16( gbgb, r01 );

            _mm_storeu_si128( dstrgb128r0++, rgb0123 );
            _mm_storeu_si128( dstrgb128r0++, rgb4567 );
            _mm_storeu_si128( dstrgb128r0++, rgb89ab );
            _mm_storeu_si128( dstrgb128r0++, rgbcdef );

            // row 1
            r00 = _mm_srai_epi16( _mm_add_epi16( y00r1, rv00 ), 6 );
            r01 = _mm_srai_epi16( _mm_add_epi16( y01r1, rv01 ), 6 );
            g00 = _mm_srai_epi16( _mm_sub_epi16( _mm_sub_epi16( y00r1, gu00 ), gv00 ), 6 );
            g01 = _mm_srai_epi16( _mm_sub_epi16( _mm_sub_epi16( y01r1, gu01 ), gv01 ), 6 );
            b00 = _mm_srai_epi16( _mm_add_epi16( y00r1, bu00 ), 6 );
            b01 = _mm_srai_epi16( _mm_add_epi16( y01r1, bu01 ), 6 );

            r00 = _mm_packus_epi16( r00, r01 );         // rrrr.. saturated
            g00 = _mm_packus_epi16( g00, g01 );         // gggg.. saturated
            b00 = _mm_packus_epi16( b00, b01 );         // bbbb.. saturated

            r01     = _mm_unpacklo_epi8(  b00,  mask_ff ); // 0r0r..
            gbgb    = _mm_unpacklo_epi8(  r00,  g00 );  // gbgb..
            rgb0123 = _mm_unpacklo_epi16( gbgb, r01 );  // 0rgb0rgb..
            rgb4567 = _mm_unpackhi_epi16( gbgb, r01 );  // 0rgb0rgb..

            r01     = _mm_unpackhi_epi8(  b00,  mask_ff );
            gbgb    = _mm_unpackhi_epi8(  r00,  g00 );
            rgb89ab = _mm_unpacklo_epi16( gbgb, r01 );
            rgbcdef = _mm_unpackhi_epi16( gbgb, r01 );

            _mm_storeu_si128( dstrgb128r1++, rgb0123 );
            _mm_storeu_si128( dstrgb128r1++, rgb4567 );
            _mm_storeu_si128( dstrgb128r1++, rgb89ab );
            _mm_storeu_si128( dstrgb128r1++, rgbcdef );

        }
    }


}


void voyuv420torgb32_16nx2n_x86( VO_U8 *dsty, VO_U8 *dstu, VO_U8 *dstv, VO_S32 in_stride, VO_S32 uin_stride, int width, int height, VO_U32 *rgb, VO_S32 srgb )
{
    __m128i y0r0, y0r1, u0, v0;
    __m128i y00r0, y01r0, y00r1, y01r1;
    __m128i u00, u01, v00, v01;
    __m128i rv00, rv01, gu00, gu01, gv00, gv01, bu00, bu01;
    __m128i r00, r01, g00, g01, b00, b01;
    __m128i rgb0123, rgb4567, rgb89ab, rgbcdef;
    __m128i gbgb;
    __m128i ysub, uvsub;
    __m128i zero, mask_ff, facy, facrv, facgu, facgv, facbu;
    __m128i *srcy128r0, *srcy128r1;
    __m128i *dstrgb128r0, *dstrgb128r1;
    __m64   *srcu64, *srcv64;
    int x, y;

    ysub  = _mm_set1_epi32( 0x00100010 );
    uvsub = _mm_set1_epi32( 0x00800080 );
    
    facy  = _mm_set1_epi32( 0x004a004a );
    facrv = _mm_set1_epi32( 0x00660066 );
    facgu = _mm_set1_epi32( 0x00190019 );
    facgv = _mm_set1_epi32( 0x00340034 );
    facbu = _mm_set1_epi32( 0x00810081 );
    
    zero  = _mm_set1_epi32( 0x00000000 );

    for( y = 0; y < height; y += 2 ) {

        srcy128r0 = (__m128i *)(dsty + in_stride*y);
        srcy128r1 = (__m128i *)(dsty + in_stride*y + in_stride);
        srcu64 = (__m64 *)(dstu + uin_stride*(y/2));
        srcv64 = (__m64 *)(dstv + uin_stride*(y/2));

        dstrgb128r0 = (__m128i *)(rgb + srgb*y);
        dstrgb128r1 = (__m128i *)(rgb + srgb*y + srgb);

        for( x = 0; x < width; x += 16 ) {

            u0 = _mm_loadl_epi64( (__m128i *)srcu64 ); srcu64++;
            v0 = _mm_loadl_epi64( (__m128i *)srcv64 ); srcv64++;

            y0r0 = _mm_loadu_si128( srcy128r0++ );
            y0r1 = _mm_loadu_si128( srcy128r1++ );

            // constant y factors
            y00r0 = _mm_mullo_epi16( _mm_sub_epi16( _mm_unpacklo_epi8( y0r0, zero ), ysub ), facy );
            y01r0 = _mm_mullo_epi16( _mm_sub_epi16( _mm_unpackhi_epi8( y0r0, zero ), ysub ), facy );
            y00r1 = _mm_mullo_epi16( _mm_sub_epi16( _mm_unpacklo_epi8( y0r1, zero ), ysub ), facy );
            y01r1 = _mm_mullo_epi16( _mm_sub_epi16( _mm_unpackhi_epi8( y0r1, zero ), ysub ), facy );

            // expand u and v so they're aligned with y values
            u0  = _mm_unpacklo_epi8( u0,  zero );
            u00 = _mm_sub_epi16( _mm_unpacklo_epi16( u0, u0 ), uvsub );
            u01 = _mm_sub_epi16( _mm_unpackhi_epi16( u0, u0 ), uvsub );

            v0  = _mm_unpacklo_epi8( v0,  zero );
            v00 = _mm_sub_epi16( _mm_unpacklo_epi16( v0, v0 ), uvsub );
            v01 = _mm_sub_epi16( _mm_unpackhi_epi16( v0, v0 ), uvsub );

            // common factors on both rows.
            rv00 = _mm_mullo_epi16( facrv, v00 );
            rv01 = _mm_mullo_epi16( facrv, v01 );
            gu00 = _mm_mullo_epi16( facgu, u00 );
            gu01 = _mm_mullo_epi16( facgu, u01 );
            gv00 = _mm_mullo_epi16( facgv, v00 );
            gv01 = _mm_mullo_epi16( facgv, v01 );
            bu00 = _mm_mullo_epi16( facbu, u00 );
            bu01 = _mm_mullo_epi16( facbu, u01 );

            // row 0
            r00 = _mm_srai_epi16( _mm_add_epi16( y00r0, rv00 ), 6 );
            r01 = _mm_srai_epi16( _mm_add_epi16( y01r0, rv01 ), 6 );
            g00 = _mm_srai_epi16( _mm_sub_epi16( _mm_sub_epi16( y00r0, gu00 ), gv00 ), 6 );
            g01 = _mm_srai_epi16( _mm_sub_epi16( _mm_sub_epi16( y01r0, gu01 ), gv01 ), 6 );
            b00 = _mm_srai_epi16( _mm_add_epi16( y00r0, bu00 ), 6 );
            b01 = _mm_srai_epi16( _mm_add_epi16( y01r0, bu01 ), 6 );

            r00 = _mm_packus_epi16( r00, r01 );         // rrrr.. saturated
            g00 = _mm_packus_epi16( g00, g01 );         // gggg.. saturated
            b00 = _mm_packus_epi16( b00, b01 );         // bbbb.. saturated

            r01     = _mm_unpacklo_epi8(  r00,  zero ); // 0r0r..
            gbgb    = _mm_unpacklo_epi8(  b00,  g00 );  // gbgb..
            rgb0123 = _mm_unpacklo_epi16( gbgb, r01 );  // 0rgb0rgb..
            rgb4567 = _mm_unpackhi_epi16( gbgb, r01 );  // 0rgb0rgb..

            r01     = _mm_unpackhi_epi8(  r00,  zero );
            gbgb    = _mm_unpackhi_epi8(  b00,  g00 );
            rgb89ab = _mm_unpacklo_epi16( gbgb, r01 );
            rgbcdef = _mm_unpackhi_epi16( gbgb, r01 );

            _mm_storeu_si128( dstrgb128r0++, rgb0123 );
            _mm_storeu_si128( dstrgb128r0++, rgb4567 );
            _mm_storeu_si128( dstrgb128r0++, rgb89ab );
            _mm_storeu_si128( dstrgb128r0++, rgbcdef );

            // row 1
            r00 = _mm_srai_epi16( _mm_add_epi16( y00r1, rv00 ), 6 );
            r01 = _mm_srai_epi16( _mm_add_epi16( y01r1, rv01 ), 6 );
            g00 = _mm_srai_epi16( _mm_sub_epi16( _mm_sub_epi16( y00r1, gu00 ), gv00 ), 6 );
            g01 = _mm_srai_epi16( _mm_sub_epi16( _mm_sub_epi16( y01r1, gu01 ), gv01 ), 6 );
            b00 = _mm_srai_epi16( _mm_add_epi16( y00r1, bu00 ), 6 );
            b01 = _mm_srai_epi16( _mm_add_epi16( y01r1, bu01 ), 6 );

            r00 = _mm_packus_epi16( r00, r01 );         // rrrr.. saturated
            g00 = _mm_packus_epi16( g00, g01 );         // gggg.. saturated
            b00 = _mm_packus_epi16( b00, b01 );         // bbbb.. saturated

            r01     = _mm_unpacklo_epi8(  r00,  zero ); // 0r0r..
            gbgb    = _mm_unpacklo_epi8(  b00,  g00 );  // gbgb..
            rgb0123 = _mm_unpacklo_epi16( gbgb, r01 );  // 0rgb0rgb..
            rgb4567 = _mm_unpackhi_epi16( gbgb, r01 );  // 0rgb0rgb..

            r01     = _mm_unpackhi_epi8(  r00,  zero );
            gbgb    = _mm_unpackhi_epi8(  b00,  g00 );
            rgb89ab = _mm_unpacklo_epi16( gbgb, r01 );
            rgbcdef = _mm_unpackhi_epi16( gbgb, r01 );

            _mm_storeu_si128( dstrgb128r1++, rgb0123 );
            _mm_storeu_si128( dstrgb128r1++, rgb4567 );
            _mm_storeu_si128( dstrgb128r1++, rgb89ab );
            _mm_storeu_si128( dstrgb128r1++, rgbcdef );

        }
    }


}

#endif
