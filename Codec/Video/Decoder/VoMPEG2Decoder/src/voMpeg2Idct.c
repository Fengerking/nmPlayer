/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include "voMpegIdct.h"



#define NEWWEIGHT1 0xa74   // 2048*sqrt(2)*cos(1*pi/8) 
#define NEWWEIGHT3 0x454   // 2048*sqrt(2)*cos(3*pi/8) 

#define CAL_LINE_FOUR_HALF(){\
	d0 = (block[0] << 11) + 65536;\
	d2 = (d2 << 11);\
	d4 = NEWWEIGHT3 * (d1 + d3);\
	d1 = d4 + (NEWWEIGHT1 - NEWWEIGHT3) * d1;\
	d3 = d4 - (NEWWEIGHT1 + NEWWEIGHT3) * d3;\
	d4 = d0 + d2;\
	d0 -= d2;\
	d2 = (d4 + d1) >> 17;\
	d4 = (d4 - d1) >> 17;\
	d1 = (d0 + d3) >> 17;\
	d0 = (d0 - d3) >> 17;\
}

VO_VOID  IdctBlock4x4Two_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride)
{
	VO_S32 *block_ptr=(VO_S32*)block;
	VO_U32 i;

	for(i = 0; i < 4; i++){
		IdctColunm8(block + i);
	}

	for(i = 0; i < 4; i++){
		IdctLine4(block + i*16,dest,src);
		dest+=dest_stride;
		src+=src_stride;
	}

	block_ptr[0] = block_ptr[1] =
	block_ptr[4] = block_ptr[5] =
	block_ptr[8] = block_ptr[9] =
	block_ptr[12] = block_ptr[13] =
	block_ptr[16] = block_ptr[17] =
	block_ptr[20] = block_ptr[21] =
	block_ptr[24] = block_ptr[25] =
	block_ptr[28] = block_ptr[29] = 0;
	block_ptr[31] = 0;
}

VO_VOID  IdctBlock8x4_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride)
{
	VO_S32 *block_ptr=(VO_S32*)block;
	VO_U32 i;

	for(i = 0; i < 8; i++){
		IdctColunm8(block + i);
	}

	for(i = 0; i < 4; i++){
		IdctLine8(block+i*16,dest,src);
		dest+=dest_stride;
		src +=src_stride;
	}

	block_ptr[0] = block_ptr[1] = block_ptr[2] = block_ptr[3] = 
	block_ptr[4] = block_ptr[5] = block_ptr[6] = block_ptr[7] = 
	block_ptr[8] = block_ptr[9] = block_ptr[10] = block_ptr[11] = 
	block_ptr[12] = block_ptr[13] = block_ptr[14] = block_ptr[15] = 
	block_ptr[16] = block_ptr[17] = block_ptr[18] = block_ptr[19] = 
	block_ptr[20] = block_ptr[21] = block_ptr[22] = block_ptr[23] = 
	block_ptr[24] = block_ptr[25] = block_ptr[26] = block_ptr[27] = 
	block_ptr[28] = block_ptr[29] = block_ptr[30] = block_ptr[31] = 0;
}

static VO_VOID IdctLine4Half(VO_S16 *block, VO_U8 *dst, const VO_U8 *src)
{
	VO_S32 d0,d1,d2,d3,d4;
	VO_U32 no_zero_flage;

	d1 = block[1];
	d2 = block[2];
	d3 = block[3];

	no_zero_flage = d1|d2|d3;

	if (!no_zero_flage){
		d0 = (block[0] + 32) >> 6;
		if (!src){
 			d0 = SAT(d0);

// 			d0 &= 255;
			d0 |= d0 << 8;
			d0 |= d0 << 16;

			((VO_U32*)dst)[0] = d0;
			return;
		}
		d2 = d4 = d1 = d0;
	}else{
		CAL_LINE_FOUR_HALF()
	}

	if (src){
		d2 += src[0];
		d1 += src[1];
		d0 += src[2];
		d4 += src[3];
	}

	if ((d2|d1|d0|d4)>>8){
		d2 = SAT(d2);
		d1 = SAT(d1);
		d0 = SAT(d0);
		d4 = SAT(d4);
	}

	((VO_U32*)dst)[0] = ((VO_U8)d2)|(((VO_U8)d1) << 8)|(((VO_U8)d0)<<16)|(((VO_U8)d4)<<24);
}

VO_VOID IdctBlock4x8Two_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride)
{
	VO_S32 *block_ptr=(VO_S32*)block;
	VO_U32 i;

	for(i = 0; i < 4; i++){
		IdctColunm8(block + i);
	}

	for(i = 0; i < 8; i++){
		IdctLine4Half(block+i*8,dest,src);
		dest+=dest_stride;
		src+=src_stride;
	}

	block_ptr[0] = block_ptr[1] = block_ptr[2] = block_ptr[3] = 
	block_ptr[4] = block_ptr[5] = block_ptr[6] = block_ptr[7] = 
	block_ptr[8] = block_ptr[9] = block_ptr[10] = block_ptr[11] = 
	block_ptr[12] = block_ptr[13] = block_ptr[14] = block_ptr[15] = 
	block_ptr[16] = block_ptr[17] = block_ptr[18] = block_ptr[19] = 
	block_ptr[20] = block_ptr[21] = block_ptr[22] = block_ptr[23] = 
	block_ptr[24] = block_ptr[25] = block_ptr[26] = block_ptr[27] = 
	block_ptr[28] = block_ptr[29] = block_ptr[30] = block_ptr[31] = 0;
}


