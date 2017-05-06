/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef _MPEG4_QPEL_H_
#define _MPEG4_QPEL_H_
#include "voMpeg4DecGlobal.h"
#include "voMpegMC.h"

#ifdef ASP_QPEL 
/*****************************************************************************

I(0)  1  2  3    I 
4     5  5  7
8     9  10 11
12    13 14 15
I                I

I = Integer pixels

****************************************************************************/

#if defined(VOARMV6) || defined(VOARMV7)
extern void Arm11InterQ1(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void Arm11InterQ2(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void Arm11InterQ3(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void Arm11InterQ4(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void Arm11InterQ8(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void Arm11InterQ12(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);

extern void Arm11InterBQ1(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void Arm11InterBQ2(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void Arm11InterBQ3(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void Arm11InterBQ4(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void Arm11InterBQ8(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void Arm11InterBQ12(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);

#define MCMpeg4BlockQpel2		Arm11InterQ2
#define MCMpeg4BlockQpel1		Arm11InterQ1
#define MCMpeg4BlockQpel3		Arm11InterQ3
#define MCMpeg4BlockQpel8		Arm11InterQ8
#define MCMpeg4BlockQpel4		Arm11InterQ4
#define MCMpeg4BlockQpel12		Arm11InterQ12


#define MCMpeg4BlockQpel2Add	Arm11InterBQ2
#define MCMpeg4BlockQpel1Add	Arm11InterBQ1
#define MCMpeg4BlockQpel3Add	Arm11InterBQ3
#define MCMpeg4BlockQpel8Add	Arm11InterBQ8
#define MCMpeg4BlockQpel4Add	Arm11InterBQ4
#define MCMpeg4BlockQpel12Add	Arm11InterBQ12
#else

extern void MCMpeg4BlockQpel1_C (VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void MCMpeg4BlockQpel2_C (VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void MCMpeg4BlockQpel3_C (VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void MCMpeg4BlockQpel4_C (VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void MCMpeg4BlockQpel8_C (VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void MCMpeg4BlockQpel12_C(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);

extern void MCMpeg4BlockQpel1Add_C (VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void MCMpeg4BlockQpel2Add_C (VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void MCMpeg4BlockQpel3Add_C (VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void MCMpeg4BlockQpel4Add_C (VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void MCMpeg4BlockQpel8Add_C (VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);
extern void MCMpeg4BlockQpel12Add_C(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd);

#define MCMpeg4BlockQpel1		MCMpeg4BlockQpel1_C
#define MCMpeg4BlockQpel2		MCMpeg4BlockQpel2_C
#define MCMpeg4BlockQpel3	    MCMpeg4BlockQpel3_C
#define MCMpeg4BlockQpel4		MCMpeg4BlockQpel4_C
#define MCMpeg4BlockQpel8		MCMpeg4BlockQpel8_C
#define MCMpeg4BlockQpel12	    MCMpeg4BlockQpel12_C

#define MCMpeg4BlockQpel1Add	MCMpeg4BlockQpel1Add_C
#define MCMpeg4BlockQpel2Add	MCMpeg4BlockQpel2Add_C
#define MCMpeg4BlockQpel3Add	MCMpeg4BlockQpel3Add_C
#define MCMpeg4BlockQpel4Add	MCMpeg4BlockQpel4Add_C
#define MCMpeg4BlockQpel8Add	MCMpeg4BlockQpel8Add_C
#define MCMpeg4BlockQpel12Add	MCMpeg4BlockQpel12Add_C
#endif

static void INLINE
MCMpeg4BlockQpel(VO_U8 *src, VO_U8 *dst, VOCONST VO_S32 src_stride, VOCONST VO_S32 dst_stride, VO_S32 quads, VOCONST VO_U32 rounding)			  					  
{
	VO_U8 tmp[8*9];

	switch(quads) {
	case 0:
		AllCopyBlock[0](src, dst, src_stride, dst_stride,8);
		break;
	case 1:
		MCMpeg4BlockQpel1(src, dst, src_stride, dst_stride, 8, rounding);
		break;
	case 2:
		MCMpeg4BlockQpel2(src, dst, src_stride, dst_stride, 8, rounding);
		break;
	case 3:
		MCMpeg4BlockQpel3(src, dst, src_stride, dst_stride, 8, rounding);
		break;
	case 4:
		MCMpeg4BlockQpel4(src, dst, src_stride, dst_stride, 8, rounding);
		break;
	case 5:
		MCMpeg4BlockQpel1(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel4(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 6:
		MCMpeg4BlockQpel2(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel4(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 7:
		MCMpeg4BlockQpel3(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel4(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 8:
		MCMpeg4BlockQpel8(src, dst, src_stride, dst_stride, 8, rounding);
		break;
	case 9:
		MCMpeg4BlockQpel1(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel8(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 10:
		MCMpeg4BlockQpel2(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel8(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 11:
		MCMpeg4BlockQpel3(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel8(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 12:
		MCMpeg4BlockQpel12(src, dst, src_stride, dst_stride, 8, rounding);
		break;
	case 13:
		MCMpeg4BlockQpel1(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel12(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 14:
		MCMpeg4BlockQpel2(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel12(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 15:
		MCMpeg4BlockQpel3(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel12(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	}
}



static void INLINE
MCMpeg4BlockQpelAdd(VO_U8 *src, VO_U8 *dst, VOCONST VO_S32 src_stride, VOCONST VO_S32 dst_stride, VO_S32 quads, VOCONST VO_U32 rounding)					  
{
	VO_U8 tmp[8*9];

	switch(quads) {
	case 0:
		AvgBlockb_C(src, dst, src_stride, dst_stride,8); 
		break;
	case 1:
		MCMpeg4BlockQpel1Add(src, dst, src_stride, dst_stride, 8, rounding);
		break;
	case 2:
		MCMpeg4BlockQpel2Add(src, dst, src_stride, dst_stride, 8, rounding);
		break;
	case 3:
		MCMpeg4BlockQpel3Add(src, dst, src_stride, dst_stride, 8, rounding);
		break;
	case 4:
		MCMpeg4BlockQpel4Add(src, dst, src_stride, dst_stride, 8, rounding);
		break;
	case 5:
		MCMpeg4BlockQpel1(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel4Add(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 6:
		MCMpeg4BlockQpel2(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel4Add(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 7:
		MCMpeg4BlockQpel3(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel4Add(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 8:
		MCMpeg4BlockQpel8Add(src, dst, src_stride, dst_stride, 8, rounding);
		break;
	case 9:
		MCMpeg4BlockQpel1(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel8Add(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 10:
		MCMpeg4BlockQpel2(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel8Add(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 11:
		MCMpeg4BlockQpel3(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel8Add(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 12:
		MCMpeg4BlockQpel12Add(src, dst, src_stride, dst_stride, 8, rounding);
		break;
	case 13:
		MCMpeg4BlockQpel1(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel12Add(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 14:
		MCMpeg4BlockQpel2(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel12Add(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	case 15:
		MCMpeg4BlockQpel3(src, tmp, src_stride, 8, 9, rounding);
		MCMpeg4BlockQpel12Add(tmp, dst, 8, dst_stride, 8, rounding);
		break;
	}
}


#endif//ASP_QPEL

#endif 