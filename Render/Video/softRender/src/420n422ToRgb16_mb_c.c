
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#include "420n422ToRgb16_mb.h"
#include "ccConstant.h"
#include "voccrender.h"

static VO_S32 ditherrb[2][2] =
{{RBDITHER_P0, RBDITHER_P1}, 
{RBDITHER_P2, RBDITHER_P3}};
static VO_S32 ditherg[2][2] =
{{GDITHER_P0, GDITHER_P1}, 
{GDITHER_P2, GDITHER_P3}};

static const CC_MB_PTR cc_mb_all[3] = 
{
	cc_mb, cc_mb_16x16, cc_mb_16x16_180
};

static const CC_MB_PTR cc_mb_l90_all[2] = 
{
	cc_mb_l90, cc_mb_16x16_l90
};

static const CC_MB_PTR cc_mb_r90_all[2] = 
{
	cc_mb_r90, cc_mb_16x16_r90
};

/***************
pix_0 | pix_1
-------------
pix_2 | pix_3
****************/

//(c, y, pix_0, pix_2)

#define LOAD_DATA(a3, a7, c, y){\
	a0 = psrc_u[c] - 128;\
	a1 = psrc_v[c] - 128;\
	a3 = (psrc_y[y] - 16)*ConstY;	\
	a7 = (psrc_y[in_stride+y] - 16)*ConstY;\
	}
#define CAL_UV(){\
	a2 = (a0 * ConstU1);\
	a0 = (a1 * ConstV2 + a0 *ConstU2);\
	a1 = (a1 * ConstV1);\
	}
#define PIX_0(a3){\
	a4  = (ccClip63[((a3 + a1)>>22)])>>1;\
	a5  = (ccClip63[((a3 - a0)>>22)]);\
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;\
	a4 = (a4<<11)|(a5<<5)|a3;\
	}
#define LOAD_YPIX(a3, x){\
	a3 = (psrc_y[(x)] - 16)*ConstY;\
	}
#define PIX_1(a3){\
	a5  = (ccClip63[((a3 + a2)>>22)])>>1;\
	a4 |= (a5 << 16);\
	a5  = (ccClip63[((a3 + a1)>>22)])>>1;\
	a3  = (ccClip63[((a3 - a0)>>22)]);\
	a3 |= (a5<<6);\
	a4 |= (a3 << 21);\
	}
#define WRITE_PIX(a4, x){\
	*((VO_S32*)(out_buf+(x))) = (VO_S32)a4;\
	}

void cc_mb_16x16_180_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
				 VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8,num;
	out_buf += (out_stride<<1) * 8 - out_stride;
	do{
		VO_S32 a0, a1, a2, a3, a3_3, a5, a5_5, a7, a7_7;
		VO_U32 a4;
		//0
//	#define num 0
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

void cc_mb_16x16_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
				 VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a3_3, a4, a5, a5_5, a7, a7_7;
#if 0
			//0
			LOAD_DATA(a3, a7, 0, 0)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 1)
			PIX_1(a3)
			
			WRITE_PIX(a4, 0)
			PIX_0(a7)
			LOAD_YPIX(a7, in_stride+1)
			PIX_1(a7)
			
			WRITE_PIX(a4, out_stride)

			//1	
			LOAD_DATA(a3, a7, 1, 2)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 3)	
			PIX_1(a3)
			
			WRITE_PIX(a4, 4)
			
			PIX_0(a7)
			LOAD_YPIX(a7, in_stride+3)
			PIX_1(a7)
			
			WRITE_PIX(a4, out_stride+4)
			
			//2
			LOAD_DATA(a3, a7, 2, 4)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 5)
			PIX_1(a3)
			
			WRITE_PIX(a4, 8)
			
			PIX_0(a7)
			LOAD_YPIX(a7, in_stride+5)
			PIX_1(a7)
			
			WRITE_PIX(a4, out_stride+8)
			
			//3
			LOAD_DATA(a3, a7, 3, 6)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 7)	
			PIX_1(a3)
			
			WRITE_PIX(a4, 12)
			
			PIX_0(a7)
			LOAD_YPIX(a7, in_stride+7)
			PIX_1(a7)
			
			WRITE_PIX(a4, out_stride+12)
			
			//4
			LOAD_DATA(a3, a7, 4, 8)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 9)	
			PIX_1(a3)
			
			WRITE_PIX(a4, 16)
			
			PIX_0(a7)
			LOAD_YPIX(a7, in_stride+9)
			PIX_1(a7)
			
			WRITE_PIX(a4, out_stride+16)
			
			//5
			LOAD_DATA(a3, a7, 5, 10)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 11)	
			PIX_1(a3)
			
			WRITE_PIX(a4, 20)
			
			PIX_0(a7)
			LOAD_YPIX(a7, in_stride+11)
			PIX_1(a7)
			
			WRITE_PIX(a4, out_stride+20)
			
			//6
			LOAD_DATA(a3, a7, 6, 12)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 13)	
			PIX_1(a3)
			
			WRITE_PIX(a4, 24)
			
			PIX_0(a7)
			LOAD_YPIX(a7, in_stride+13)
			PIX_1(a7)
			
			WRITE_PIX(a4, out_stride+24)
			
			//7
			LOAD_DATA(a3, a7, 7, 14)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 15)	
			PIX_1(a3)
			
			WRITE_PIX(a4, 28)
			
			PIX_0(a7)
			LOAD_YPIX(a7, in_stride+15)
			PIX_1(a7)
			WRITE_PIX(a4, out_stride+28)
#else
		//0
	#define num 0
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
	*((VO_S32*)(out_buf+(4*num))) = (VO_S32)a4;

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
	*((VO_S32*)(out_buf+(out_stride+4*num))) = (VO_S32)a4;

	#define num 1
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
	*((VO_S32*)(out_buf+(4*num))) = (VO_S32)a4;

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
	*((VO_S32*)(out_buf+(out_stride+4*num))) = (VO_S32)a4;
	#define num 2
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
	*((VO_S32*)(out_buf+(4*num))) = (VO_S32)a4;

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
	*((VO_S32*)(out_buf+(out_stride+4*num))) = (VO_S32)a4;
	#define num 3
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
	*((VO_S32*)(out_buf+(4*num))) = (VO_S32)a4;

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
	*((VO_S32*)(out_buf+(out_stride+4*num))) = (VO_S32)a4;
	#define num 4
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
	*((VO_S32*)(out_buf+(4*num))) = (VO_S32)a4;

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
	*((VO_S32*)(out_buf+(out_stride+4*num))) = (VO_S32)a4;
	#define num 5
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
	*((VO_S32*)(out_buf+(4*num))) = (VO_S32)a4;

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
	*((VO_S32*)(out_buf+(out_stride+4*num))) = (VO_S32)a4;
	#define num 6
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
	*((VO_S32*)(out_buf+(4*num))) = (VO_S32)a4;

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
	*((VO_S32*)(out_buf+(out_stride+4*num))) = (VO_S32)a4;
	#define num 7
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
	*((VO_S32*)(out_buf+(4*num))) = (VO_S32)a4;

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
	*((VO_S32*)(out_buf+(out_stride+4*num))) = (VO_S32)a4;
#endif						
			psrc_y += (in_stride<<1);
			psrc_u += uin_stride;
			psrc_v += vin_stride;
			out_buf += (out_stride<<1);

	}while(--a6 != 0);
}


///////////////////////////////////////////////////////////////////////Rotation left 90
#undef WRITE_PIX

#define WRITE_PIX(a4, x){\
	*((VO_S32*)(out_buf+(x))) = (VO_S32)a4;\
	out_buf += (x);\
}

void cc_mb_16x16_l90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
					 VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a5_5, a7;
#if 0
		LOAD_DATA(a3, a7, 0, 0)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 1)
			PIX_1(a7)
			
			WRITE_PIX(a4, 0)
			
			PIX_0(a3)
			LOAD_YPIX(a7, in_stride+1)
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			//1	
			LOAD_DATA(a3, a7, 1, 2)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 3)	
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			PIX_0(a3)
			LOAD_YPIX(a7, in_stride+3)
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			//2
			LOAD_DATA(a3, a7, 2, 4)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 5)
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			PIX_0(a3)
			LOAD_YPIX(a7, in_stride+5)
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			//3
			LOAD_DATA(a3, a7, 3, 6)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 7)	
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			PIX_0(a3)
			LOAD_YPIX(a7, in_stride+7)
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			//4
			LOAD_DATA(a3, a7, 4, 8)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 9)	
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			PIX_0(a3)
			LOAD_YPIX(a7, in_stride+9)
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			//5
			LOAD_DATA(a3, a7, 5, 10)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 11)	
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			PIX_0(a3)
			LOAD_YPIX(a7, in_stride+11)
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			//6
			LOAD_DATA(a3, a7, 6, 12)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 13)	
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			PIX_0(a3)
			LOAD_YPIX(a7, in_stride+13)
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			//7
			LOAD_DATA(a3, a7, 7, 14)
			CAL_UV()
			PIX_0(a3)
			LOAD_YPIX(a3, 15)	
			PIX_1(a7)
			
			WRITE_PIX(a4, -out_stride)
			
			PIX_0(a3)
			LOAD_YPIX(a7, in_stride+15)
			PIX_1(a7)
			WRITE_PIX(a4, -out_stride)
#else
		//0
	#define num 0
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

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf)) = (VO_S32)a4;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;
	#define num 1
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

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;
	#define num 2
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

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;

	#define num 3
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

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;
	#define num 4
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

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;

	#define num 5
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

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;
	#define num 6
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

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;

	#define num 7
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

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(-out_stride))) = (VO_S32)a4;
	out_buf+=-out_stride;
#endif		
			psrc_y += (in_stride<<1);
			psrc_u += uin_stride;
			psrc_v += vin_stride;
	out_buf+=-out_stride;
	out_buf+=4;					
			a7 = out_stride<<4;
			out_buf += a7;
			
	}while(--a6 != 0);
}


///////////////////////////////////////////////////////////////////////Rotation right 90

#define PIX_2(a3){\
	a4  = ccClip63[((a3 + a1)>>22)]>>1;\
	a5  = ccClip63[((a3 - a0)>>22)];\
	a3  = ccClip63[((a3 + a2)>>22)]>>1;\
	a4 = (a4<<27)|(a5<<21)|(a3<<16);\
}
#define PIX_3(a3){\
	a5  = ccClip63[((a3 + a2)>>22)]>>1;\
	a4 |= a5;\
	a5  = ccClip63[((a3 + a1)>>22)]>>1;\
	a3  = ccClip63[((a3 - a0)>>22)];\
	a3 |= (a5<<6);\
	a4 |= (a3<<5);\
}

void cc_mb_16x16_r90_c(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
					 VO_U8 *out_buf, const VO_S32 out_stride, VO_S32 width, VO_S32 height,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_U32 a6 = 8;
	do{
		VO_S32 a0, a1, a2, a3, a4, a5, a5_5, a7;
#if 0
		//0
		LOAD_DATA(a3, a7, 0, 0)
			CAL_UV()
			PIX_2(a3)
			LOAD_YPIX(a3, 1)
			PIX_3(a7)
			
			WRITE_PIX(a4, 0)
			
			PIX_2(a3)
			LOAD_YPIX(a7, in_stride+1)
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			//1	
			LOAD_DATA(a3, a7, 1, 2)
			CAL_UV()
			PIX_2(a3)
			LOAD_YPIX(a3, 3)	
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			PIX_2(a3)
			LOAD_YPIX(a7, in_stride+3)
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			//2
			LOAD_DATA(a3, a7, 2, 4)
			CAL_UV()
			PIX_2(a3)
			LOAD_YPIX(a3, 5)
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			PIX_2(a3)
			LOAD_YPIX(a7, in_stride+5)
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			//3
			LOAD_DATA(a3, a7, 3, 6)
			CAL_UV()
			PIX_2(a3)
			LOAD_YPIX(a3, 7)	
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			PIX_2(a3)
			LOAD_YPIX(a7, in_stride+7)
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			//4
			LOAD_DATA(a3, a7, 4, 8)
			CAL_UV()
			PIX_2(a3)
			LOAD_YPIX(a3, 9)	
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			PIX_2(a3)
			LOAD_YPIX(a7, in_stride+9)
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			//5
			LOAD_DATA(a3, a7, 5, 10)
			CAL_UV()
			PIX_2(a3)
			LOAD_YPIX(a3, 11)	
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			PIX_2(a3)
			LOAD_YPIX(a7, in_stride+11)
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			//6
			LOAD_DATA(a3, a7, 6, 12)
			CAL_UV()
			PIX_2(a3)
			LOAD_YPIX(a3, 13)	
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			PIX_2(a3)
			LOAD_YPIX(a7, in_stride+13)
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			//7
			LOAD_DATA(a3, a7, 7, 14)
			CAL_UV()
			PIX_2(a3)
			LOAD_YPIX(a3, 15)	
			PIX_3(a7)
			
			WRITE_PIX(a4, out_stride)
			
			PIX_2(a3)
			LOAD_YPIX(a7, in_stride+15)
			PIX_3(a7)
			WRITE_PIX(a4, out_stride)
#else
	#define num 0
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
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left

	*((VO_S32*)(out_buf)) = (VO_S32)a4;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	#define num 1
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
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	#define num 2
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
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	#define num 3
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
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	#define num 4
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
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	#define num 5
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
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	#define num 6
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
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	#define num 7
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
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a3 = (psrc_y[(2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<27)|(a5<<21)|(a3<<16);//dif form left	a4 = (a4<<11)|(a5<<5)|a3;

	a7 = (psrc_y[(in_stride+2*num + 1)] - 16)*ConstY;

	a5  = (ccClip63[((a7 + a2)>>22)])>>1;
	a4 |= a5;//dif form left a4 |= (a5 << 16);
	a5  = (ccClip63[((a7 + a1)>>22)])>>1;
	a7  = (ccClip63[((a7 - a0)>>22)]);
	a7 |= (a5<<6);
	a4 |= (a7<<5);//dif form left a4 |= (a7 << 21);

	*((VO_S32*)(out_buf+(out_stride))) = (VO_S32)a4;
	out_buf+=out_stride;	
#endif		
			psrc_y += (in_stride<<1);
			psrc_u += uin_stride;
			psrc_v += vin_stride;
	out_buf+=out_stride;
	out_buf-= 4;	
			a7 = out_stride<<4;
			out_buf -= a7;
			
	}while(--a6 != 0);
}

void cc_mb(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride, 
		   VO_U8 *dst, const VO_S32 dst_stride, VO_S32 width, VO_S32 height,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	
	do{
		i = width;
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			
			a3 = *(src_v++) - 128;	
			a2 = *(src_u++) - 128;
			
			a0 = a3 * ConstV1;
			a1 = a3 * ConstV2 + a2 *ConstU2;
			a2 = a2 * ConstU1;
			a3 = ((*src_y) - 16) * ConstY;
			a4 = (*(src_y + 1) - 16) * ConstY;
			
			a3 = ((ccClip255[((a3 + a0)>>20) + RDITHER_P0]>>3)<<11)|((ccClip255[((a3 - a1)>>20) + GDITHER_P0]>>2)<<5)|(ccClip255[((a3 + a2)>>20) + BDITHER_P0]>>3);
			a4 = ((ccClip255[((a4 + a0)>>20) + RDITHER_P1]>>3)<<11)|((ccClip255[((a4 - a1)>>20) + GDITHER_P1]>>2)<<5)|(ccClip255[((a4 + a2)>>20) + BDITHER_P1]>>3);
			a4 = (a4 << 16) | a3;
			*((VO_U32*)dst) = a4;
			
			/////////////////////////////////////////////////////////////////////
			a3 = (*(src_y + src_stride) - 16) * ConstY;
			a4 = (*(src_y + src_stride + 1) - 16) * ConstY;
			src_y += 2;
			
			a3 = ((ccClip255[((a3 + a0)>>20) + RDITHER_P2]>>3)<<11)|((ccClip255[((a3 - a1)>>20) + GDITHER_P2]>>2)<<5)|(ccClip255[((a3 + a2)>>20) + BDITHER_P2]>>3);
			a4 = ((ccClip255[((a4 + a0)>>20) + RDITHER_P3]>>3)<<11)|((ccClip255[((a4 - a1)>>20) + GDITHER_P3]>>2)<<5)|(ccClip255[((a4 + a2)>>20) + BDITHER_P3]>>3);
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

void cc_mb_l90(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride, 
			   VO_U8 *dst, const VO_S32 dst_stride, VO_S32 width, VO_S32 height,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	
	do{
		i = width;
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			
			a3 = *(src_v++) - 128;	
			a2 = *(src_u++) - 128;
			
			a0 = a3 * ConstV1;
			a1 = a3 * ConstV2 + a2 *ConstU2;
			a2 = a2 * ConstU1;
			a3 = (*src_y - 16) * ConstY;
			a4 = (*(src_y + src_stride) - 16) * ConstY;
			src_y++;
			
			a3 = ((ccClip255[((a3 + a0)>>20) + RDITHER_P0]>>3)<<11)|((ccClip255[((a3 - a1)>>20) + GDITHER_P0]>>2)<<5)|(ccClip255[((a3 + a2)>>20) + BDITHER_P0]>>3);
			a4 = ((ccClip255[((a4 + a0)>>20) + RDITHER_P1]>>3)<<11)|((ccClip255[((a4 - a1)>>20) + GDITHER_P1]>>2)<<5)|(ccClip255[((a4 + a2)>>20) + BDITHER_P1]>>3);
			a4 = (a4 << 16) | a3;
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			/////////////////////////////////////////////////////////////////////
			a3 = (*(src_y) - 16) * ConstY;
			a4 = (*(src_y + src_stride) - 16) * ConstY;
			src_y ++;
			
			a3 = ((ccClip255[((a3 + a0)>>20) + RDITHER_P2]>>3)<<11)|((ccClip255[((a3 - a1)>>20) + GDITHER_P2]>>2)<<5)|(ccClip255[((a3 + a2)>>20) + BDITHER_P2]>>3);
			a4 = ((ccClip255[((a4 + a0)>>20) + RDITHER_P3]>>3)<<11)|((ccClip255[((a4 - a1)>>20) + GDITHER_P3]>>2)<<5)|(ccClip255[((a4 + a2)>>20) + BDITHER_P3]>>3);
			a4 = (a4 << 16) | a3;
			*((VO_U32*)dst) = a4;
			dst -= dst_stride;
			
		}while((i-=2) != 0);
		
		i = (width >> 1);
		src_u -= i;
		src_v -= i;
		//i = (src_stride>>1);
		src_u += uin_stride;
		src_v += vin_stride;
		src_y -= width;
		src_y += (src_stride << 1);
		
		dst += (dst_stride*width);
		dst += 4;
	}while((height-=2) != 0);
}

void cc_mb_r90(VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, const VO_S32 src_stride, 
			   VO_U8 *dst, const VO_S32 dst_stride, VO_S32 width, VO_S32 height,
				 const VO_S32 uin_stride, const VO_S32 vin_stride)
{
	VO_S32 i;
	
	do{
		i = width;
		do{
			VO_S32 a0, a1, a2, a3, a4, a5, a6, a7, a8;
			
			a3 = *(src_v++) - 128;	
			a2 = *(src_u++) - 128;
			
			a0 = a3 * ConstV1;
			a1 = a3 * ConstV2 + a2 *ConstU2;
			a2 = a2 * ConstU1;
			a4 = (*src_y - 16) * ConstY;
			a3 = (*(src_y + src_stride) - 16) * ConstY;
			src_y++;
			
			a3 = ((ccClip255[((a3 + a0)>>20) + RDITHER_P0]>>3)<<11)|((ccClip255[((a3 - a1)>>20) + GDITHER_P0]>>2)<<5)|(ccClip255[((a3 + a2)>>20) + BDITHER_P0]>>3);
			a4 = ((ccClip255[((a4 + a0)>>20) + RDITHER_P1]>>3)<<11)|((ccClip255[((a4 - a1)>>20) + GDITHER_P1]>>2)<<5)|(ccClip255[((a4 + a2)>>20) + BDITHER_P1]>>3);

			a4 = (a4 << 16) | a3;
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			/////////////////////////////////////////////////////////////////////
			a4 = (*(src_y) - 16) * ConstY;
			a3 = (*(src_y + src_stride) - 16) * ConstY;
			src_y ++;
			
			a3 = ((ccClip255[((a3 + a0)>>20) + RDITHER_P2]>>3)<<11)|((ccClip255[((a3 - a1)>>20) + GDITHER_P2]>>2)<<5)|(ccClip255[((a3 + a2)>>20) + BDITHER_P2]>>3);
			a4 = ((ccClip255[((a4 + a0)>>20) + RDITHER_P3]>>3)<<11)|((ccClip255[((a4 - a1)>>20) + GDITHER_P3]>>2)<<5)|(ccClip255[((a4 + a2)>>20) + BDITHER_P3]>>3);

			a4 = (a4 << 16) | a3;
			*((VO_U32*)dst) = a4;
			dst += dst_stride;
			
		}while((i-=2) != 0);
		
		i = (width >> 1);
		src_u -= i;
		src_v -= i;
		//i = (src_stride>>1);
		src_u += uin_stride;
		src_v += vin_stride;
		src_y -= width;
		src_y += (src_stride << 1);
		
		dst -= (dst_stride*width);
		dst -= 4;
	}while((height-=2) != 0);
}

void cc_yuv420_disable(unsigned char *y, unsigned char *u, unsigned char *v,
					   int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
					   unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	VO_S32 vx = 0, skip = 0, width, height;
	VO_U8 *psrc_y = y, *psrc_u = u, *psrc_v = v, *dst, *start_out_buf;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	start_out_buf = out_buf;
	
	do{
		vx = out_width;
		out_buf = start_out_buf;	
		do{
			if(mb_skip) skip = *(mb_skip++);
			if(!skip){
				width = vx < 16 ? vx : 16;
				height = out_height < 16 ? out_height : 16;
				cc_mb_all[(width==16)&&(height==16)](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width, height, uin_stride, vin_stride);
			}
			
			psrc_y += 16;
			psrc_u += 8;
			psrc_v += 8;
			out_buf += 32;
		}while((vx -= 16)>0);
		
		psrc_y = y = y + (in_stride << 4);
		psrc_u = u = u + (uin_stride << 3);
		psrc_v = v = v + (vin_stride << 3);
		start_out_buf += (out_stride << 4);
	}while((out_height -= 16)>0);	
}
void cc_yuv420_disable_180(unsigned char *y, unsigned char *u, unsigned char *v,
					   int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
					   unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	VO_S32 vx = 0, skip = 0, width, height;
	VO_U8 *psrc_y = y, *psrc_u = u, *psrc_v = v, *dst, *start_out_buf;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	start_out_buf = out_buf + out_stride * (out_height - 16);
	
	do{
		vx = out_width;
		out_buf = start_out_buf + out_stride - 32;	
		do{
			if(mb_skip) skip = *(mb_skip++);
			if(!skip){
				width = vx < 16 ? vx : 16;
				height = out_height < 16 ? out_height : 16;
				cc_mb_all[((width==16)&&(height==16)) * 2](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width, height, uin_stride, vin_stride);
			}
			
			psrc_y += 16;
			psrc_u += 8;
			psrc_v += 8;
			out_buf -= 32;
		}while((vx -= 16)>0);
		
		psrc_y = y = y + (in_stride << 4);
		psrc_u = u = u + (uin_stride << 3);
		psrc_v = v = v + (vin_stride << 3);
		start_out_buf -= (out_stride << 4);
	}while((out_height -= 16)>0);	
}

void cc_yuv420_r90(unsigned char *y, unsigned char *u, unsigned char *v,
				   int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
				   unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	VO_S32 vx = 0, skip = 0, width, height;
	VO_U8 *psrc_y=y, *psrc_u=u, *psrc_v=v, *dst, *start_out_buf;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end	
	
	start_out_buf = out_buf + (out_height << 1) - 4;
	
	/*prepare resize*/	
	do{
		vx = out_width;
		out_buf = start_out_buf;	
		do{
			if(mb_skip) skip = *(mb_skip++);
			if(!skip){
				width = vx < 16 ? vx : 16;
				height = out_height < 16 ? out_height : 16;
				cc_mb_r90_all[(width==16)&&(height==16)](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width, height, uin_stride, vin_stride);
			}
			
			psrc_y += 16;
			psrc_u += 8;
			psrc_v += 8;
			out_buf += (out_stride << 4);
			
		}while((vx -= 16)>0);
		psrc_y = y = y + (in_stride << 4);
		psrc_u = u = u + (uin_stride << 3);
		psrc_v = v = v + (vin_stride << 3);
		start_out_buf -= 32;
	}while((out_height -= 16)>0);	
}

void cc_yuv420_l90(unsigned char *y, unsigned char *u, unsigned char *v,
				   int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
				   unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	VO_S32 vx = 0, skip = 0, width, height;
	VO_U8 *psrc_y=y, *psrc_u=u, *psrc_v=v, *dst, *start_out_buf;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	uin_stride = Pin_stride[1];
	vin_stride = Pin_stride[2];	
//add end

	start_out_buf = out_buf + (out_width - 1)* out_stride;
	
	do{
		vx = out_width;
		out_buf = start_out_buf;
		do{
			if(mb_skip) skip = *(mb_skip++);
			if(!skip){
				width = vx < 16 ? vx : 16;
				height = out_height < 16 ? out_height : 16;
				cc_mb_l90_all[(width==16)&&(height==16)](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width, height, uin_stride, vin_stride);
			}
			psrc_y += 16;
			psrc_u += 8;
			psrc_v += 8;
			out_buf -= (out_stride << 4);
		}while((vx -= 16)>0);
		
		psrc_y = y = y + (in_stride << 4);
		psrc_u = u = u + (uin_stride << 3);
		psrc_v = v = v + (vin_stride << 3);
		start_out_buf += 32;
	}while((out_height -= 16)>0);	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cc_rgb24_disable(unsigned char *y, unsigned char *u, unsigned char *v,
					  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
					  unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	VO_S32 a4;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride;//, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	//uin_stride = Pin_stride[1];
	//vin_stride = Pin_stride[2];	
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
}


void cc_rgb24_l90(unsigned char *y, unsigned char *u, unsigned char *v,
				  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
				  unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	VO_S32 a4;

int in_stride = Pin_stride[0];

	VO_U8 *psrc = y + in_stride;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
//	int in_stride;//, uin_stride, vin_stride;
//	in_stride  = Pin_stride[0];
	//uin_stride = Pin_stride[1];
	//vin_stride = Pin_stride[2];	
//add end	
	
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
}

void cc_rgb24_r90(unsigned char *y, unsigned char *u, unsigned char *v,
				  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
				  unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	VO_S32 a4;

	int in_stride = Pin_stride[0];

	VO_U8 *psrc = y + in_stride;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
//	int in_stride;//, uin_stride, vin_stride;
//	in_stride  = Pin_stride[0];
	//uin_stride = Pin_stride[1];
	//vin_stride = Pin_stride[2];	
//add end	
	
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
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cc_rgb565_disable(unsigned char *y, unsigned char *u, unsigned char *v,
					   int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
					   unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	VO_S32 a4 = out_width << 1;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
	int in_stride;//, uin_stride, vin_stride;
	in_stride  = Pin_stride[0];
	//uin_stride = Pin_stride[1];
	//vin_stride = Pin_stride[2];	
//add end		
	do{
		memcpy(out_buf, y, a4);

		out_buf += out_stride;
		
		y += in_stride;
		
	}while(--out_height != 0);
}

void cc_rgb565_l90(unsigned char *y, unsigned char *u, unsigned char *v,
				   int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
				   unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	VO_S32 a4;
	int in_stride = Pin_stride[0];
	VO_U8 *psrc = y + in_stride;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
//	int in_stride;//, uin_stride, vin_stride;
//	in_stride  = Pin_stride[0];
	//uin_stride = Pin_stride[1];
	//vin_stride = Pin_stride[2];	
//add end	
	
	out_buf += (out_width - 1) * out_stride;
	do{
		a4 = out_width;
		do {
			VO_U32 a0, a1, a2, a3;
			//0
			a0 = *((VO_U32*)y);
			y += 4;

			a1 = *((VO_U32*)psrc);
			psrc += 4;

			a2 = (a0&0xffff) | (a1<<16);
			a3 = (a0>>16) | (a1&0xffff0000);

			*((VO_U32*)(out_buf)) = a2;
			out_buf -= out_stride;
			
			*((VO_U32*)(out_buf)) = a3;
			out_buf -= out_stride;
		}while((a4-=2)>0);
		
		out_buf += (out_width * out_stride);
		out_buf += 4;
		
		y -= (out_width<<1);
		y += (in_stride<<1);
		psrc = y + in_stride;
		
	}while((out_height-=2)>0);
}

void cc_rgb565_r90(unsigned char *y, unsigned char *u, unsigned char *v,
				   int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
				   unsigned char *out_buf, int out_stride, unsigned char *mb_skip)
{
	VO_S32 a4;
	int in_stride = Pin_stride[0];
	VO_U8 *psrc = y + in_stride;
//add	
//	int * Pin_stride
//	, uin_stride, vin_stride
//	int in_stride;//, uin_stride, vin_stride;
//	in_stride  = Pin_stride[0];
	//uin_stride = Pin_stride[1];
	//vin_stride = Pin_stride[2];	
//add end	
	
	out_buf += (out_height << 1) - 4;
	do{
		a4 = out_width;
		do {
			VO_U32 a0, a1, a2, a3, a5, a6, a7;
			//0
			a1 = *((VO_U32*)y);
			y += 4;

			a0 = *((VO_U32*)psrc);
			psrc += 4;

			a2 = (a0&0xffff) | (a1<<16);
			a3 = (a0>>16) | (a1&0xffff0000);

			*((VO_U32*)(out_buf)) = a2;
			out_buf += out_stride;
			
			*((VO_U32*)(out_buf)) = a3;
			out_buf += out_stride;
		}while((a4-=2)>0);
		
		out_buf -= (out_width * out_stride);
		out_buf -= 4;
		
		y -= (out_width<<1);
		y += (in_stride<<1);
		psrc = y + in_stride;
		
	}while((out_height-=2)>0);
}
