/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include "voMpegMC.h"


#define PREPARE_AVERAGE4_BYTE(s0,s1,s2,s3)				\
{											\
	VO_U32 t0 = s0&s2;							\
	VO_U32 t1 = s1&s3;							\
	s2 = s0^s2;								\
	s3 = s1^s3;								\
	s0 = t0 + ((s2>>1)&0x7F7F7F7F);			\
	s1 = t1 + ((s3>>1)&0x7F7F7F7F);			\
}

#define ADD_BYTE_ALIGN(s0,s1,d0,d1)			\
{								\
	d0 =*((VO_U32*)(dst));		\
	d1 =*((VO_U32*)(dst + 4));		\
	*((VO_U32*)(dst))     = (s0|d0) - (((s0^d0)>>1) & 0x7F7F7F7F);	\
	*((VO_U32*)(dst + 4)) = (s1|d1) - (((s1^d1)>>1) & 0x7F7F7F7F);	\
}

#define AVERAGE2_BYTE(s0,s1,s2,s3,t0,t1)					\
{											\
	t0 = (s0|s2) - (((s0^s2)>>1) & 0x7F7F7F7F);	\
	t1 = (s1|s3) - (((s1^s3)>>1) & 0x7F7F7F7F);	\
}

#define AVERAGE4_BYTE(s0,s1,s2,s3,s4,s5,s6,s7){\
	VO_U32 t0 = s0^s4;						\
	VO_U32 t1 = s1^s5;						\
	s0 = (s0&s4) + ((t0>>1)&0x7F7F7F7F) + (((s2&s6)|t0)&0x01010101);			\
	s1 = (s1&s5) + ((t1>>1)&0x7F7F7F7F) + (((s3&s7)|t1)&0x01010101);			\
}

//no align
#define LOAD_BYTE(s0,s1,org){\
	s0  = *(src + org);\
	s0 |= ((*(src+org+1))<<8);\
	s0 |= ((*(src+org+2))<<16);\
	s0 |= ((*(src+org+3))<<24);\
	s1  = *(src+org+4);\
	s1 |= ((*(src+org+5))<<8);\
	s1 |= ((*(src+org+6))<<16);\
	s1 |= ((*(src+org+7))<<24);\
}

//align
#define LOAD_BYTE_ALIGN(s0, s1){\
	s0  = *((VO_U32*)(src));\
	s1  = *((VO_U32*)(src + 4));\
}

#define LOAD_16BYTE_ALIGN(s0, s1, s2, s3){\
	s0  = *((VO_U32*)(src));\
	s1  = *((VO_U32*)(src + 4));\
	s2  = *((VO_U32*)(src + 8));\
	s3  = *((VO_U32*)(src + 12));\
}

#define SAVE_BYTE_ALIGN(d0, d1, t0, t1){\
	*((VO_U32*)(dst))      =d0;\
	*((VO_U32*)(dst + 4))  =d1;\
	if (0)\
	{\
		d0 = t0;\
		d1 = t1;\
	}\
}

#define SAVE_16BYTE_ALIGN(d0, d1, d2, d3){\
	*((VO_U32*)(dst))      =d0;\
	*((VO_U32*)(dst + 4))  =d1;\
	*((VO_U32*)(dst + 8))  =d2;\
	*((VO_U32*)(dst + 12)) =d3;\
}

#define AVERAGE2_ROUND_BYTE(s0,s1,s2,s3,t0,t1)	\
{											\
	t0 = (s0&s2) + (((s0^s2)>>1) & 0x7F7F7F7F);	\
	t1 = (s1&s3) + (((s1^s3)>>1) & 0x7F7F7F7F);	\
}

#define AVERAGE4_ROUND_BYTE(s0,s1,s2,s3,s4,s5,s6,s7)			\
{											\
	VO_U32 t0 = s0^s4;						\
	VO_U32 t1 = s1^s5;						\
	s0 = (s0&s4) + ((t0>>1)&0x7F7F7F7F) + ((s2|s6)&t0&0x01010101);			\
	s1 = (s1&s5) + ((t1>>1)&0x7F7F7F7F) + ((s3|s7)&t1&0x01010101);			\
}


#define MC_BLOCK_C(fun_name, add, avg2, avg4) \
VO_VOID MCMpeg##fun_name##_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)\
{ \
	for(; height > 0; height -= 2){\
		VO_U32 s0,s1,s2,s3;\
		LOAD_BYTE(s0, s1, 0)\
		src += src_stride;\
		add##_BYTE_ALIGN(s0, s1, s2, s3)\
		dst += dst_stride;		\
		LOAD_BYTE(s0, s1, 0)\
		src += src_stride;\
		add##_BYTE_ALIGN(s0, s1,s2,s3)\
		dst += dst_stride;\
	}\
}\
VO_VOID MCMpeg##fun_name##Hor_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)\
{\
	for(; height > 0; height--){\
		VO_U32 s0,s1,s2,s3,s4,s5;	\
		LOAD_BYTE(s0, s1, 0)\
		LOAD_BYTE(s2, s3, 1)\
		src += src_stride;\
		avg2##_BYTE(s0, s1, s2, s3, s4, s5)\
		add##_BYTE_ALIGN(s4, s5, s0, s1)\
		dst += dst_stride;\
	}\
}\
VO_VOID MCMpeg##fun_name##Ver_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)\
{\
	VO_U32 s0,s1,s2,s3,s4,s5;\
	LOAD_BYTE(s0, s1, 0)\
	src += src_stride;\
	for(; height > 0; height -= 2){\
		LOAD_BYTE(s2, s3, 0)\
		src += src_stride;\
		avg2##_BYTE(s0, s1, s2, s3, s4, s5)\
		add##_BYTE_ALIGN(s4, s5, s0, s1)\
		dst += dst_stride;\
		LOAD_BYTE(s0, s1, 0)\
		src += src_stride;\
		avg2##_BYTE(s2, s3, s0, s1, s4, s5)\
		add##_BYTE_ALIGN(s4, s5, s2, s3)\
		dst += dst_stride ;\
	}\
}\
VO_VOID MCMpeg##fun_name##HorVer_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)\
{\
	VO_U32 s0,s1,s2,s3,s4,s5,s6,s7;\
	LOAD_BYTE(s0, s1, 0)\
	LOAD_BYTE(s2, s3, 1)\
	src += src_stride;\
	PREPARE_AVERAGE4_BYTE(s0, s1, s2, s3)\
	for(; height > 0; height -= 2){\
		LOAD_BYTE(s4, s5, 0)\
		LOAD_BYTE(s6, s7, 1)\
		src += src_stride;\
		PREPARE_AVERAGE4_BYTE(s4, s5, s6, s7)\
		avg4##_BYTE(s0, s1, s2, s3, s4, s5, s6, s7)\
		add##_BYTE_ALIGN(s0, s1, s2, s3)		\
		dst += dst_stride;\
		LOAD_BYTE(s0, s1, 0)\
		LOAD_BYTE(s2, s3, 1)\
		src += src_stride;\
		PREPARE_AVERAGE4_BYTE(s0, s1, s2, s3)\
		avg4##_BYTE(s4, s5, s6, s7, s0, s1, s2, s3)\
		add##_BYTE_ALIGN(s4, s5, s6, s7)\
		dst += dst_stride;\
	}\
}\

MC_BLOCK_C(Block,SAVE,AVERAGE2,AVERAGE4)
MC_BLOCK_C(BlockAdd,ADD,AVERAGE2,AVERAGE4)

MC_BLOCK_C(BlockRd,SAVE,AVERAGE2_ROUND,AVERAGE4_ROUND)
MC_BLOCK_C(BlockAddRd,ADD,AVERAGE2_ROUND,AVERAGE4_ROUND)

VO_VOID CopyBlock16x16_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)
{
	for(; height > 0; height--){
		VO_U32 s0,s1,s2,s3;
		LOAD_16BYTE_ALIGN(s0, s1, s2, s3)
			src += src_stride;
		SAVE_16BYTE_ALIGN(s0, s1, s2, s3 )
			dst += dst_stride;
	}
}

VO_VOID CopyBlock8x8_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)
{
	for(; height > 0; height -= 2){
		VO_U32 s0,s1,s2,s3;
		LOAD_BYTE_ALIGN(s0, s1)
			src += src_stride;
		LOAD_BYTE_ALIGN(s2, s3)
			src += src_stride;
		SAVE_BYTE_ALIGN(s0, s1, 0, 0)
			dst += dst_stride;
		SAVE_BYTE_ALIGN(s2, s3, 0, 0)
			dst += dst_stride;
	}
}

// Dst[p] = (((Src[p] + Dst[p]+1) >> 1) 
VO_VOID AvgBlockb_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height)
{
	VO_U32 s0,s1,s2,s3,s4,s5;
	for(; height > 0; height--){
		LOAD_BYTE(s0,s1,0)
		s2=dst[0]|(dst[1]<<8)|((dst[2]|((VO_U8)dst[3]<<8))<<16);
		s3=dst[4]|(dst[5]<<8)|((dst[6]|((VO_U8)dst[7]<<8))<<16); 
		AVERAGE2_BYTE(s0,s1,s2,s3,s4,s5)
		SAVE_BYTE_ALIGN(s4,s5,0,0)
		dst += dst_stride;
		src += src_stride;
	}
}

const COPYBLOCK_PTR AllCopyBlock[8] = //[8x8/16x16][Rounding][x][y]
{
#if defined(VOARMV4)
	Armv4Inter8x8, Armv4Inter8x8H, Armv4Inter8x8V, Armv4Inter8x8HV ,
	Armv4Inter8x8, Armv4Inter8x8HRD, Armv4Inter8x8VRD, Armv4Inter8x8HVRD
#elif defined(VOARMV6)
	Arm11Inter8x8, Arm11Inter8x8H, Arm11Inter8x8V, Arm11Inter8x8HV,
	Arm11Inter8x8, Arm11Inter8x8HRD, Arm11Inter8x8VRD, Arm11Inter8x8HVRD
#elif defined(VOARMV7)
	MPEG4DEC_VO_Armv7Inter8x8, MPEG4DEC_VO_Armv7Inter8x8H, MPEG4DEC_VO_Armv7Inter8x8V, MPEG4DEC_VO_Armv7Inter8x8HV,
	MPEG4DEC_VO_Armv7Inter8x8, MPEG4DEC_VO_Armv7Inter8x8HRD, MPEG4DEC_VO_Armv7Inter8x8VRD, MPEG4DEC_VO_Armv7Inter8x8HVRD
#elif defined(VOWMMX)
	WmmxInter8x8, WmmxInter8x8H, WmmxInter8x8V, WmmxInter8x8HV,
	WmmxInter8x8, WmmxInter8x8HRD, WmmxInter8x8VRD, WmmxInter8x8HVRD
#else
	MCMpegBlock_C, MCMpegBlockHor_C, MCMpegBlockVer_C, MCMpegBlockHorVer_C,
	MCMpegBlock_C, MCMpegBlockRdHor_C, MCMpegBlockRdVer_C, MCMpegBlockRdHorVer_C
#endif
};

const addblock  AllAddBlock[8] = 
{
#if defined(VOARMV4)
	Armv4Inter2_8x8 , Armv4Inter2_8x8H, Armv4Inter2_8x8V, Armv4Inter2_8x8HV,
	NULL, NULL, NULL, NULL
#elif defined(VOARMV6)
	Arm11Inter2_8x8 , Arm11Inter2_8x8H, Arm11Inter2_8x8V, Arm11Inter2_8x8HV,
	NULL, NULL, NULL, NULL
#elif defined(VOARMV7)
	MPEG4DEC_VO_Armv7Inter2_8x8 , MPEG4DEC_VO_Armv7Inter2_8x8H, MPEG4DEC_VO_Armv7Inter2_8x8V, MPEG4DEC_VO_Armv7Inter2_8x8HV,
	NULL, NULL, NULL, NULL
#elif defined(VOWMMX)
	wmmx_addblk, wmmx_addblkh, wmmx_addblkv, wmmx_addblkhv,
	NULL, NULL, NULL, NULL
#else
	MCMpegBlockAdd_C, MCMpegBlockAddHor_C, MCMpegBlockAddVer_C, MCMpegBlockAddHorVer_C,
	NULL, NULL, NULL, NULL
#endif
};

