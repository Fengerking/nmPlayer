/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#include "voMpeg2Decoder.h"
#include "voMpegMC.h"


const COPYBLOCK_PTR AllCopyBlock[8] = //[8x8/16x16][Rounding][x][y]
{
#if defined(VOARMV4) || defined(VOARMV6)
	 ArmInter8x8, ArmInter8x8H, ArmInter8x8V, ArmInter8x8HV,
	 NULL, NULL, NULL, NULL
#elif defined(VOWMMX)
	 wmmx_copyblk, wmmx_copyblkh, wmmx_copyblkv, wmmx_copyblkhv,
	 NULL, NULL, NULL, NULL
#elif defined(VOARMv7)
	MPEG2DEC_VO_Armv7Inter8x8, MPEG2DEC_VO_Armv7Inter8x8H, MPEG2DEC_VO_Armv7Inter8x8V, MPEG2DEC_VO_Armv7Inter8x8HV,
	NULL, NULL, NULL, NULL
#else
	 MCMpegBlock_C, MCMpegBlockHor_C, MCMpegBlockVer_C, MCMpegBlockHorVer_C,
	 NULL, NULL, NULL, NULL
#endif
};


const addblock  AllAddBlock[8] = 
{
#if defined(VOARMV4) || defined(VOARMV6)
	ArmInter2_8x8, ArmInter2_8x8H, ArmInter2_8x8V, ArmInter2_8x8HV,
	NULL, NULL, NULL, NULL
#elif defined(VOWMMX)
	wmmx_addblk, wmmx_addblkh, wmmx_addblkv, wmmx_addblkhv,
	NULL, NULL, NULL, NULL
#elif defined(VOARMv7)
	MPEG2DEC_VO_Armv7Inter2_8x8, MPEG2DEC_VO_Armv7Inter2_8x8H, MPEG2DEC_VO_Armv7Inter2_8x8V,MPEG2DEC_VO_Armv7Inter2_8x8HV,
	NULL, NULL, NULL, NULL
#else
	MCMpegBlockAdd_C, MCMpegBlockAddHor_C, MCMpegBlockAddVer_C, MCMpegBlockAddHorVer_C,
	NULL, NULL, NULL, NULL
#endif
};

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
	if (0) \
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

//MC_BLOCK_C(BlockRd,SAVE,AVERAGE2_ROUND,AVERAGE4_ROUND)
//MC_BLOCK_C(BlockAddRd,ADD,AVERAGE2_ROUND,AVERAGE4_ROUND)

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

#ifdef BFRAME_NOERR
const COPYBLOCK_PTR BAllCopyBlock8x4[8] = //[8x8/16x16][Rounding][x][y]
{
#if defined(VOARMV4) || defined(VOARMV6)
	//BArmInter8x4, BArmInter8x4H, BArmInter8x4V, BArmInter8x4HV
	BArmInter8x4, BArmInter8x4H,  ArmInter8x8V, ArmInter8x8HV,
	NULL, NULL, NULL, NULL
#elif defined(VOWMMX)
	BArmInter8x4, BArmInter8x4H, BArmInter8x4V, BArmInter8x4HV,
	NULL, NULL, NULL, NULL
#elif defined(VOARMv7)
	MPEG2DEC_VO_BArmv7Inter8x4, MPEG2DEC_VO_BArmv7Inter8x4H, MPEG2DEC_VO_ Armv7Inter8x8V, MPEG2DEC_VO_Armv7Inter8x8HV,
	NULL, NULL, NULL, NULL
#else
	MCMpegBlock_C, MCMpegBlockHor_C, BMCMpegBlockVer8x4_C, BMCMpegBlockHorVer8x4_C,
	NULL, NULL, NULL, NULL
#endif
};
const addblock  BAllAddBlock8x4[8] = 
{
#if defined(VOARMV4) || defined(VOARMV6)
	//BArmInter2_8x4, BArmInter2_8x4H, BArmInter2_8x4V, BArmInter2_8x4HV
	BArmInter2_8x4, BArmInter2_8x4H, ArmInter2_8x8V, ArmInter2_8x8HV,
	NULL, NULL, NULL, NULL
#elif defined(VOWMMX)
	BArmInter2_8x4, BArmInter2_8x4H, BArmInter2_8x4V, BArmInter2_8x4HV,
	NULL, NULL, NULL, NULL
#elif defined(VOARMv7)
	MPEG2DEC_VO_BArmv7Inter2_8x4, MPEG2DEC_VO_BArmv7Inter2_8x4H, MPEG2DEC_VO_Armv7Inter2_8x8V, MPEG2DEC_VO_Armv7Inter2_8x8HV,
	NULL, NULL, NULL, NULL
#else
	MCMpegBlockAdd_C, MCMpegBlockAddHor_C, BMCMpegBlockAddVer8x4_C, BMCMpegBlockAddHorVer8x4_C,
	NULL, NULL, NULL, NULL
#endif
};

#define MC_BLOCK4X8_C(fun_name, add, avg2, avg4) \
VO_VOID BMCMpeg##fun_name##Ver8x4_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)\
{\
	VO_U32 s0,s1,s2,s3,s4,s5;\
	VO_S32 temp_srcpitch;\
	temp_srcpitch = (src_stride>>1);\
	for(; height > 0; height--){\
	LOAD_BYTE(s0, s1, 0)\
	src += temp_srcpitch;\
	LOAD_BYTE(s2, s3, 0)\
	src += temp_srcpitch;\
	avg2##_BYTE(s0, s1, s2, s3, s4, s5)\
	add##_BYTE_ALIGN(s4, s5, s0, s1)\
	dst += dst_stride;\
	}\
}\
VO_VOID BMCMpeg##fun_name##HorVer8x4_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)\
{\
	VO_U32 s0,s1,s2,s3,s4,s5,s6,s7;\
	VO_S32 temp_srcpitch;\
	temp_srcpitch = (src_stride>>1);\
	for(; height > 0; height--){\
	LOAD_BYTE(s0, s1, 0)\
	LOAD_BYTE(s2, s3, 1)\
	PREPARE_AVERAGE4_BYTE(s0, s1, s2, s3)\
	src += temp_srcpitch;\
	LOAD_BYTE(s4, s5, 0)\
	LOAD_BYTE(s6, s7, 1)\
	PREPARE_AVERAGE4_BYTE(s4, s5, s6, s7)\
	src += temp_srcpitch;\
	avg4##_BYTE(s0, s1, s2, s3, s4, s5, s6, s7)\
	add##_BYTE_ALIGN(s0, s1, s2, s3)		\
	dst += dst_stride;\
	}\
}\

MC_BLOCK4X8_C(Block,SAVE,AVERAGE2,AVERAGE4)
MC_BLOCK4X8_C(BlockAdd,ADD,AVERAGE2,AVERAGE4)
#endif //BFRAME_NOERR

const COPYBLOCK_PTR NewAllCopyBlock4_8[8] = //[8x8/16x16][Rounding][x][y]
{
#if defined(VOARMV4) || defined(VOARMV6)
	ArmInter4x8, MCMpegBlock4x8_01,ArmInter4x8H, MCMpegBlock4x8_03, 
	ArmInter4x8V, MCMpegBlock4x8_11,ArmInter4x8HV, MCMpegBlock4x8_13
#elif defined(VOWMMX)
	wmmx4x8_copyblk, MCMpegBlock4x8_01,wmmx4x8_copyblkh,MCMpegBlock4x8_03,
	wmmx4x8_copyblkv,MCMpegBlock4x8_11, wmmx4x8_copyblkhv,MCMpegBlock4x8_13
#elif defined(VOARMv7)
	//MPEG2DEC_VO_Armv7Inter4x8, MCMpegBlock4x8_01,MPEG2DEC_VO_Armv7Inter4x8H, MCMpegBlock4x8_03, 
	//MPEG2DEC_VO_Armv7Inter4x8V, MCMpegBlock4x8_11,MPEG2DEC_VO_Armv7Inter4x8HV, MCMpegBlock4x8_13
	//InterpolateBlock4x8, MCMpegBlock4x8_01, InterpolateBlockHor4x8, MCMpegBlock4x8_03,
	//InterpolateBlockVer4x8, MCMpegBlock4x8_11, InterpolateBlockHorVer4x8, MCMpegBlock4x8_13

	MCMpegBlock4x8_C, MCMpegBlock4x8_01, MCMpegBlock4x8Hor_C, MCMpegBlock4x8_03,//huwei 20110603 Bugs which should fixed
	MCMpegBlock4x8Ver_C, MCMpegBlock4x8_11, MCMpegBlock4x8HorVer_C, MCMpegBlock4x8_13
#else
	MCMpegBlock4x8_C, MCMpegBlock4x8_01, MCMpegBlock4x8Hor_C, MCMpegBlock4x8_03,
	MCMpegBlock4x8Ver_C, MCMpegBlock4x8_11, MCMpegBlock4x8HorVer_C, MCMpegBlock4x8_13
#endif
};
const addblock  NewAllAddBlock4x8[8] = 
{
#if defined(VOARMV4) || defined(VOARMV6)
	ArmInter2_4x8, MCMpegBlockAdd4x8_01,ArmInter2_4x8H,MCMpegBlockAdd4x8_03,
	ArmInter2_4x8V,MCMpegBlockAdd4x8_11,ArmInter2_4x8HV,MCMpegBlockAdd4x8_13
#elif defined(VOWMMX)
	wmmx4x8_addblk, MCMpegBlockAdd4x8_01,wmmx4x8_addblkh,MCMpegBlockAdd4x8_03, 
	wmmx4x8_addblkv,MCMpegBlockAdd4x8_11,wmmx4x8_addblkhv,MCMpegBlockAdd4x8_13
#elif defined(VOARMv7)
	//MPEG2DEC_VO_Armv7Inter2_4x8, MCMpegBlockAdd4x8_01,MPEG2DEC_VO_Armv7Inter2_4x8H,MCMpegBlockAdd4x8_03,
	//MPEG2DEC_VO_Armv7Inter2_4x8V,MCMpegBlockAdd4x8_11,MPEG2DEC_VO_Armv7Inter2_4x8HV,MCMpegBlockAdd4x8_13

	MCMpegBlockAdd4x8_C, MCMpegBlockAdd4x8_01,MCMpegBlockAdd4x8Hor_C,MCMpegBlockAdd4x8_03,
	MCMpegBlockAdd4x8Ver_C, MCMpegBlockAdd4x8_11, MCMpegBlockAdd4x8HorVer_C, MCMpegBlockAdd4x8_13
#else
	MCMpegBlockAdd4x8_C, MCMpegBlockAdd4x8_01,MCMpegBlockAdd4x8Hor_C,MCMpegBlockAdd4x8_03,
	MCMpegBlockAdd4x8Ver_C, MCMpegBlockAdd4x8_11, MCMpegBlockAdd4x8HorVer_C, MCMpegBlockAdd4x8_13
#endif
};

#undef LOAD_BYTE
#undef LOAD_BYTE_ALIGN
#undef SAVE_BYTE_ALIGN
#undef AVERAGE2_BYTE
#undef LOAD_BYTE_ALIGN
#undef PREPARE_AVERAGE4_BYTE
#undef AVERAGE4_BYTE
#undef ADD_BYTE_ALIGN

#define LOAD_BYTE(s0,s1,org){\
	s0  = *(src + org);\
	s0 |= ((*(src+org+1))<<8);\
	s0 |= ((*(src+org+2))<<16);\
	s0 |= ((*(src+org+3))<<24);\
	if (0)\
	{\
		s0 = s1;\
	}\
}

#define SAVE_BYTE_ALIGN(d0, d1, t0, t1){\
	(*((VO_U32*)(dst)))      =d0;\
	if (0)\
	{\
		(*((VO_U32*)(dst)))      =d1;\
		(*((VO_U32*)(dst)))      =t0;\
		(*((VO_U32*)(dst)))      =t1;\
	}\
}\

#define AVERAGE2_BYTE(s0,s1,s2,s3,t0,t1){\
	t0 = ~((~(s0|s2)) + (((s0^s2)>>1) & 0x7F7F7F7F));	\
	if (0)\
	{\
		s0 = s1;\
		s2 = s3;\
		t0 = t1;\
	}\
}

#define PREPARE_AVERAGE4_BYTE(s0,s1,s2,s3){\
	VO_U32 t0 = s0^s2;							\
	s0 = (s0&s2) + ((t0>>1)&0x7F7F7F7F);			\
	s2 = t0;									\
	if (0)\
	{\
		s0 = s1;\
		s2 = s3;\
	}\
}

#define AVERAGE4_BYTE(s0,s1,s2,s3,s4,s5,s6,s7){\
	VO_U32 t0 = s0^s4;						\
	s0 = (s0&s4) + ((t0>>1)&0x7F7F7F7F) + (((s2&s6)|t0)&0x01010101);			\
	if (0)\
	{\
		s0 = s1;\
		s2 = s3;\
		s4 = s5;\
		s6 = s7;\
	}\
}

#define ADD_BYTE_ALIGN(s0, s1, d0, d1){\
	d0 = *((VO_U32*)(dst));		\
	*((VO_U32*)(dst))     = ~((~(s0|d0)) + (((s0^d0)>>1) & 0x7F7F7F7F));	\
	if (0)\
	{\
		s0 = s1;\
		d0 = d1;\
	}\
}

#define LOAD_4BYTE_ALIGN(s0){\
	s0  = *((VO_U32*)(src));\
}

#define LOAD_8BYTE_ALIGN(s0, s1){\
	s0  = *((VO_U32*)(src));\
	s1  = *((VO_U32*)(src+4));\
}

#define SAVE_8BYTE_ALIGN(d0, d1){\
	(*((VO_U32*)(dst)))      =d0;\
	(*((VO_U32*)(dst+4)))    =d1;\
}\

MC_BLOCK_C(Block4x8,SAVE,AVERAGE2,AVERAGE4)
MC_BLOCK_C(BlockAdd4x8,ADD,AVERAGE2,AVERAGE4)


VO_VOID CopyBlock8x16_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride,VO_S32 height)
{
	VO_S32 i;
	VO_U32 s0,s1;

	for(i = 0; i < height; i++){
		LOAD_8BYTE_ALIGN(s0,  s1)
		SAVE_8BYTE_ALIGN(s0, s1)
		dst += dst_stride;
		src += src_stride;
	}
}

VO_VOID CopyBlock4x8_C(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)
{
	VO_S32 i;
	VO_U32 s0,s1;

	for(i = 0; i < height; i += 2){
		LOAD_4BYTE_ALIGN(s0)
		src += src_stride;
		LOAD_4BYTE_ALIGN(s1)
		src += src_stride;
		SAVE_BYTE_ALIGN(s0, 0, 0, 0)
		dst += dst_stride;
		SAVE_BYTE_ALIGN(s1, 0, 0, 0)
		dst += dst_stride;
	}
}

VO_VOID MCMpegBlock4x8_01(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)
{
	VO_S32 i; 
	VO_S32 height_temp;
	VO_U8 temp[5];
	height_temp = (height>>1);
	for (i=0;i<height_temp;++i) { 
		temp[0] = src[0];
		temp[1] = src[1];
		temp[2] = src[2];
		temp[3] = src[3];
		temp[4] = src[4];
		dst[0] = (VO_U8)(((temp[0]*3 + temp[1] + 2) >> 2)); 
		dst[1] = (VO_U8)(((temp[1]*3 + temp[2] + 2) >> 2)); 
		dst[2] = (VO_U8)(((temp[2]*3 + temp[3] + 2) >> 2)); 
		dst[3] = (VO_U8)(((temp[3]*3 + temp[4] + 2) >> 2)); 
		dst += dst_stride; 
		src += src_stride; 

		temp[0] = src[0];
		temp[1] = src[1];
		temp[2] = src[2];
		temp[3] = src[3];
		temp[4] = src[4];
		dst[0] = (VO_U8)(((temp[0]*3 + temp[1] + 2) >> 2)); 
		dst[1] = (VO_U8)(((temp[1]*3 + temp[2] + 2) >> 2)); 
		dst[2] = (VO_U8)(((temp[2]*3 + temp[3] + 2) >> 2)); 
		dst[3] = (VO_U8)(((temp[3]*3 + temp[4] + 2) >> 2)); 
		dst += dst_stride; 
		src += src_stride; 
	} 
}
VO_VOID MCMpegBlock4x8_03(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)
{
	VO_S32 i;
	VO_S32 height_temp;
	VO_U8 temp[5];
	height_temp = (height>>1);
	for (i=0;i<height_temp;++i) { 
		temp[0] = src[0];
		temp[1] = src[1];
		temp[2] = src[2];
		temp[3] = src[3];
		temp[4] = src[4];
		dst[0] = (VO_U8)(((temp[0] + temp[1]*3 + 2) >> 2)); 
		dst[1] = (VO_U8)(((temp[1] + temp[2]*3 + 2) >> 2)); 
		dst[2] = (VO_U8)(((temp[2] + temp[3]*3 + 2) >> 2)); 
		dst[3] = (VO_U8)(((temp[3] + temp[4]*3 + 2) >> 2)); 
		dst += dst_stride; 
		src += src_stride; 

		temp[0] = src[0];
		temp[1] = src[1];
		temp[2] = src[2];
		temp[3] = src[3];
		temp[4] = src[4];
		dst[0] = (VO_U8)(((temp[0] + temp[1]*3 + 2) >> 2)); 
		dst[1] = (VO_U8)(((temp[1] + temp[2]*3 + 2) >> 2)); 
		dst[2] = (VO_U8)(((temp[2] + temp[3]*3 + 2) >> 2)); 
		dst[3] = (VO_U8)(((temp[3] + temp[4]*3 + 2) >> 2)); 
		dst += dst_stride; 
		src += src_stride; 
	} 
}
VO_VOID MCMpegBlock4x8_11(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)
{
	VO_S32 i; 
	VO_S32 height_temp;
	VO_U8 temp1[5];
	VO_U8 temp2[5];
	temp1[0] = src[0];
	temp1[1] = src[1];
	temp1[2] = src[2];
	temp1[3] = src[3];
	temp1[4] = src[4];
	height_temp = (height>>1);
	for (i=0;i<height_temp;++i) { 
		src += src_stride;
		temp2[0] = src[0];
		temp2[1] = src[1];
		temp2[2] = src[2];
		temp2[3] = src[3];
		temp2[4] = src[4];
		dst[0] = (VO_U8)((((temp1[0] + temp2[0])*3 + temp1[1] + temp2[1] + 4) >> 3)); 
		dst[1] = (VO_U8)((((temp1[1] + temp2[1])*3 + temp1[2] + temp2[2] + 4) >> 3)); 
		dst[2] = (VO_U8)((((temp1[2] + temp2[2])*3 + temp1[3] + temp2[3] + 4) >> 3)); 
		dst[3] = (VO_U8)((((temp1[3] + temp2[2])*3 + temp1[4] + temp2[4] + 4) >> 3));
		dst += dst_stride; 
		
		src += src_stride;
		temp1[0] = src[0];
		temp1[1] = src[1];
		temp1[2] = src[2];
		temp1[3] = src[3];
		temp1[4] = src[4];
		dst[0] = (VO_U8)((((temp2[0] + temp1[0])*3 + temp2[1] + temp1[1] + 4) >> 3)); 
		dst[1] = (VO_U8)((((temp2[1] + temp1[1])*3 + temp2[2] + temp1[2] + 4) >> 3)); 
		dst[2] = (VO_U8)((((temp2[2] + temp1[2])*3 + temp2[3] + temp1[3] + 4) >> 3)); 
		dst[3] = (VO_U8)((((temp2[3] + temp1[2])*3 + temp2[4] + temp1[4] + 4) >> 3)); 
		dst += dst_stride; 
	} 
}
VO_VOID MCMpegBlock4x8_13(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)
{
	VO_S32 i;
	VO_S32 height_temp;
	VO_U8 temp1[5];
	VO_U8 temp2[5];
	height_temp = (height>>1);
	temp1[0] = src[0];
	temp1[1] = src[1];
	temp1[2] = src[2];
	temp1[3] = src[3];
	temp1[4] = src[4];
	for (i=0;i<height_temp;++i) { 
		src += src_stride; 
		temp2[0] = src[0];
		temp2[1] = src[1];
		temp2[2] = src[2];
		temp2[3] = src[3];
		temp2[4] = src[4];
		dst[0] = (VO_U8)(((temp1[0] + (temp1[1] + temp2[1])*3 + temp2[0] + 4) >> 3)); 
		dst[1] = (VO_U8)(((temp1[1] + (temp1[2] + temp2[2])*3 + temp2[1] + 4) >> 3)); 
		dst[2] = (VO_U8)(((temp1[2] + (temp1[3] + temp2[3])*3 + temp2[2] + 4) >> 3)); 
		dst[3] = (VO_U8)(((temp1[3] + (temp1[4] + temp2[4])*3 + temp2[3] + 4) >> 3)); 
		dst += dst_stride; 
		
		src += src_stride; 
		temp1[0] = src[0];
		temp1[1] = src[1];
		temp1[2] = src[2];
		temp1[3] = src[3];
		temp1[4] = src[4];
		dst[0] = (VO_U8)(((temp2[0] + (temp2[1] + temp1[1])*3 + temp1[0] + 4) >> 3)); 
		dst[1] = (VO_U8)(((temp2[1] + (temp2[2] + temp1[2])*3 + temp1[1] + 4) >> 3)); 
		dst[2] = (VO_U8)(((temp2[2] + (temp2[3] + temp1[3])*3 + temp1[2] + 4) >> 3)); 
		dst[3] = (VO_U8)(((temp2[3] + (temp2[4] + temp1[4])*3 + temp1[3] + 4) >> 3)); 
		dst += dst_stride;  
	} 
}
VO_VOID MCMpegBlockAdd4x8_01(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)
{
	VO_S32 i;
	VO_S32 height_temp;
	VO_U8 temp[5];
	height_temp = (height>>1);
	for (i=0;i<height_temp;++i){
		temp[0] = src[0];
		temp[1] = src[1];
		temp[2] = src[2];
		temp[3] = src[3];
		temp[4] = src[4];
		dst[0] = (VO_U8)((((temp[0]*3 + temp[1] + 2) >> 2) + dst[0] + 1) >> 1); 
		dst[1] = (VO_U8)((((temp[1]*3 + temp[2] + 2) >> 2) + dst[1] + 1) >> 1); 
		dst[2] = (VO_U8)((((temp[2]*3 + temp[3] + 2) >> 2) + dst[2] + 1) >> 1); 
		dst[3] = (VO_U8)((((temp[3]*3 + temp[4] + 2) >> 2) + dst[3] + 1) >> 1); 
		dst += dst_stride;
		src += src_stride;

		temp[0] = src[0];
		temp[1] = src[1];
		temp[2] = src[2];
		temp[3] = src[3];
		temp[4] = src[4];
		dst[0] = (VO_U8)((((temp[0]*3 + temp[1] + 2) >> 2) + dst[0] + 1) >> 1); 
		dst[1] = (VO_U8)((((temp[1]*3 + temp[2] + 2) >> 2) + dst[1] + 1) >> 1); 
		dst[2] = (VO_U8)((((temp[2]*3 + temp[3] + 2) >> 2) + dst[2] + 1) >> 1); 
		dst[3] = (VO_U8)((((temp[3]*3 + temp[4] + 2) >> 2) + dst[3] + 1) >> 1); 
		dst += dst_stride;
		src += src_stride;
	}
}
VO_VOID MCMpegBlockAdd4x8_03(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)
{
	VO_S32 i;
	VO_S32 height_temp;
	VO_U8 temp[5];
	height_temp = (height>>1);
	for (i=0;i<height_temp;++i){
		temp[0] = src[0];
		temp[1] = src[1];
		temp[2] = src[2];
		temp[3] = src[3];
		temp[4] = src[4];
		dst[0] = (VO_U8)((((temp[0] + temp[1]*3 + 2) >> 2) + dst[0] + 1) >> 1); 
		dst[1] = (VO_U8)((((temp[1] + temp[2]*3 + 2) >> 2) + dst[1] + 1) >> 1); 
		dst[2] = (VO_U8)((((temp[2] + temp[3]*3 + 2) >> 2) + dst[2] + 1) >> 1); 
		dst[3] = (VO_U8)((((temp[3] + temp[4]*3 + 2) >> 2) + dst[3] + 1) >> 1); 
		dst += dst_stride;
		src += src_stride;

		temp[0] = src[0];
		temp[1] = src[1];
		temp[2] = src[2];
		temp[3] = src[3];
		temp[4] = src[4];
		dst[0] = (VO_U8)((((temp[0] + temp[1]*3 + 2) >> 2) + dst[0] + 1) >> 1); 
		dst[1] = (VO_U8)((((temp[1] + temp[2]*3 + 2) >> 2) + dst[1] + 1) >> 1); 
		dst[2] = (VO_U8)((((temp[2] + temp[3]*3 + 2) >> 2) + dst[2] + 1) >> 1); 
		dst[3] = (VO_U8)((((temp[3] + temp[4]*3 + 2) >> 2) + dst[3] + 1) >> 1); 
		dst += dst_stride;
		src += src_stride;
	}
}
VO_VOID MCMpegBlockAdd4x8_11(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)
{
	VO_S32 i;
	VO_S32 height_temp;
	VO_U8 temp1[5];
	VO_U8 temp2[5];
	height_temp = (height>>1);
	temp1[0] = src[0];
	temp1[1] = src[1];
	temp1[2] = src[2];
	temp1[3] = src[3];
	temp1[4] = src[4];
	for (i=0;i<height_temp;++i){
		src += src_stride;
		temp2[0] = src[0];
		temp2[1] = src[1];
		temp2[2] = src[2];
		temp2[3] = src[3];
		temp2[4] = src[4];
		dst[0] = (VO_U8)(((((temp1[0] + temp2[0])*3 + temp1[1] + temp2[1] + 4) >> 3) + dst[0] + 1) >> 1); 
		dst[1] = (VO_U8)(((((temp1[1] + temp2[1])*3 + temp1[2] + temp2[2] + 4) >> 3) + dst[1] + 1) >> 1); 
		dst[2] = (VO_U8)(((((temp1[2] + temp2[2])*3 + temp1[3] + temp2[3] + 4) >> 3) + dst[2] + 1) >> 1); 
		dst[3] = (VO_U8)(((((temp1[3] + temp2[3])*3 + temp1[4] + temp2[4] + 4) >> 3) + dst[3] + 1) >> 1); 
		dst += dst_stride;
		
		src += src_stride;
		temp1[0] = src[0];
		temp1[1] = src[1];
		temp1[2] = src[2];
		temp1[3] = src[3];
		temp1[4] = src[4];
		dst[0] = (VO_U8)(((((temp2[0] + temp1[0])*3 + temp2[1] + temp1[1] + 4) >> 3) + dst[0] + 1) >> 1); 
		dst[1] = (VO_U8)(((((temp2[1] + temp1[1])*3 + temp2[2] + temp1[2] + 4) >> 3) + dst[1] + 1) >> 1); 
		dst[2] = (VO_U8)(((((temp2[2] + temp1[2])*3 + temp2[3] + temp1[3] + 4) >> 3) + dst[2] + 1) >> 1); 
		dst[3] = (VO_U8)(((((temp2[3] + temp1[3])*3 + temp2[4] + temp1[4] + 4) >> 3) + dst[3] + 1) >> 1); 
		dst += dst_stride;
	}
}
VO_VOID MCMpegBlockAdd4x8_13(VO_U8 *src, VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride, VO_S32 height)
{
	VO_S32 i;
	VO_S32 height_temp;
	VO_U8 temp1[5];
	VO_U8 temp2[5];
	height_temp = (height>>1);
	temp1[0] = src[0];
	temp1[1] = src[1];
	temp1[2] = src[2];
	temp1[3] = src[3];
	temp1[4] = src[4];
	for (i=0;i<height_temp;++i){
		src += src_stride;
		temp2[0] = src[0];
		temp2[1] = src[1];
		temp2[2] = src[2];
		temp2[3] = src[3];
		temp2[4] = src[4];
		dst[0] = (VO_U8)((((temp1[0] + (temp1[1] + temp2[1])*3 + temp2[0] + 4) >> 3) + dst[0] + 1) >> 1); 
		dst[1] = (VO_U8)((((temp1[1] + (temp1[2] + temp2[2])*3 + temp2[1] + 4) >> 3) + dst[1] + 1) >> 1); 
		dst[2] = (VO_U8)((((temp1[2] + (temp1[3] + temp2[3])*3 + temp2[2] + 4) >> 3) + dst[2] + 1) >> 1); 
		dst[3] = (VO_U8)((((temp1[3] + (temp1[4] + temp2[4])*3 + temp2[3] + 4) >> 3) + dst[3] + 1) >> 1); 
		dst += dst_stride;
		
		src += src_stride;
		temp1[0] = src[0];
		temp1[1] = src[1];
		temp1[2] = src[2];
		temp1[3] = src[3];
		temp1[4] = src[4];
		dst[0] = (VO_U8)((((temp2[0] + (temp2[1] + temp1[1])*3 + temp1[0] + 4) >> 3) + dst[0] + 1) >> 1); 
		dst[1] = (VO_U8)((((temp2[1] + (temp2[2] + temp1[2])*3 + temp1[1] + 4) >> 3) + dst[1] + 1) >> 1); 
		dst[2] = (VO_U8)((((temp2[2] + (temp2[3] + temp1[3])*3 + temp1[2] + 4) >> 3) + dst[2] + 1) >> 1); 
		dst[3] = (VO_U8)((((temp2[3] + (temp2[4] + temp1[4])*3 + temp1[3] + 4) >> 3) + dst[3] + 1) >> 1); 
		dst += dst_stride;
	}
}


