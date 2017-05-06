/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include "voMpegMC.h"

const SUBBLOCK AllSubBlock[2][4] = //[8x8/16x16][Rounding][x][y]
{
#ifdef VOWMMX
	{ SubBlock_WMMX, SubBlockHor_WMMX0, SubBlockVer_WMMX0, SubBlockHorVer_WMMX0},
	{ SubBlock_WMMX, SubBlockHor_WMMX1, SubBlockVer_WMMX1, SubBlockHorVer_WMMX1 }
#elif defined(VOARMV6)
	{ SubBlock_ARMV6, SubBlockHor_ARMV6, SubBlockVer_ARMV6, SubBlockHorVer_ARMV6 },
	{ SubBlock_ARMV6, SubBlockHorRound_ARMV6, SubBlockVerRound_ARMV6, SubBlockHorVerRound_ARMV6 }
#elif defined(VOARMV7)
	{ SubBlock_ARMV7, SubBlockHor_ARMV7, SubBlockVer_ARMV7, SubBlockHorVer_ARMV7 },
	{ SubBlock_ARMV7, SubBlockHorRound_ARMV7, SubBlockVerRound_ARMV7, SubBlockHorVerRound_ARMV7 }
#else
	{ SubBlock, SubBlockHor, SubBlockVer, SubBlockHorVer },
	{ SubBlock, SubBlockHorRound, SubBlockVerRound, SubBlockHorVerRound }
#endif
};

const COPYBLOCK AllCopyBlock[2][4] = //[8x8/16x16][Rounding][x][y]
{

#if defined(VOARMV4)
	{ MCMpegBlock_ARMV4, MCMpegBlockHor_ARMV4, MCMpegBlockVer_ARMV4, MCMpegBlockHorVer_ARMV4 },
	{ MCMpegBlock_ARMV4, MCMpegBlockRdHor_ARMV4, MCMpegBlockRdVer_ARMV4, MCMpegBlockRdHorVer_ARMV4 }
#elif defined(VOARMV6)
	{ MCMpegBlock_ARMV6, MCMpegBlockHor_ARMV6, MCMpegBlockVer_ARMV6, MCMpegBlockHorVer_ARMV6 },
	{ MCMpegBlock_ARMV6, MCMpegBlockRdHor_ARMV6, MCMpegBlockRdVer_ARMV6, MCMpegBlockRdHorVer_ARMV6 }
#elif defined(VOARMV7)
	{ MCMpegBlock_ARMV7, MCMpegBlockHor_ARMV7, MCMpegBlockVer_ARMV7, MCMpegBlockHorVer_ARMV7 },
	{ MCMpegBlock_ARMV7, MCMpegBlockRdHor_ARMV7, MCMpegBlockRdVer_ARMV7, MCMpegBlockRdHorVer_ARMV7 }
#elif defined(VOWMMX)
	{ WmmxInter8x8, WmmxInter8x8H, WmmxInter8x8V, WmmxInter8x8HV },
	{ WmmxInter8x8, WmmxInter8x8HRD, WmmxInter8x8VRD, WmmxInter8x8HVRD }
#else
	{ MCMpegBlock_C, MCMpegBlockHor_C, MCMpegBlockVer_C, MCMpegBlockHorVer_C },
	{ MCMpegBlock_C, MCMpegBlockRdHor_C, MCMpegBlockRdVer_C, MCMpegBlockRdHorVer_C }
#endif
};

const ADDBLOCK  AllAddBlock[4] = 
{
#if defined(VOARMV4)
	MCMpegBlockAdd_ARMV4 , MCMpegBlockAddHor_ARMV4, MCMpegBlockAddVer_ARMV4, MCMpegBlockAddHorVer_ARMV4
#elif defined(VOARMV6)
	MCMpegBlockAdd_ARMV6 , MCMpegBlockAddHor_ARMV6, MCMpegBlockAddVer_ARMV6, MCMpegBlockAddHorVer_ARMV6
#elif defined(VOARMV7)
	MCMpegBlockAdd_ARMV7 , MCMpegBlockAddHor_ARMV7, MCMpegBlockAddVer_ARMV7, MCMpegBlockAddHorVer_ARMV7
#elif defined(VOWMMX)
	wmmx_addblk, wmmx_addblkh, wmmx_addblkv, wmmx_addblkhv
#else
	MCMpegBlockAdd_C, MCMpegBlockAddHor_C, MCMpegBlockAddVer_C, MCMpegBlockAddHorVer_C
#endif
};


#define PREPARE_AVERAGE4_BYTE(s0,s1,s2,s3){\
	VO_U32 t0 = s0^s2;							\
	VO_U32 t1 = s1^s3;							\
	s0 = (s0&s2) + ((t0>>1)&0x7F7F7F7F);			\
	s1 = (s1&s3) + ((t1>>1)&0x7F7F7F7F);			\
	s2 = t0;									\
	s3 = t1;									\
}

#define ADD_BYTE_ALIGN(s0,s1,d0,d1){\
	d0 = *((VO_U32*)(dst));		\
	d1 = *((VO_U32*)(dst + 4));		\
	*((VO_U32*)(dst))     = ~((~(s0|d0)) + (((s0^d0)>>1) & 0x7F7F7F7F));	\
	*((VO_U32*)(dst + 4)) = ~((~(s1|d1)) + (((s1^d1)>>1) & 0x7F7F7F7F));	\
	}

#define AVERAGE2_BYTE(s0,s1,s2,s3,t0,t1){\
	t0 = ~((~(s0|s2)) + (((s0^s2)>>1) & 0x7F7F7F7F));	\
	t1 = ~((~(s1|s3)) + (((s1^s3)>>1) & 0x7F7F7F7F));	\
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
		VO_U32 s0,s1,s2,s3=0;\
		LOAD_BYTE(s0, s1, 0)\
		src += src_stride;\
		add##_BYTE_ALIGN(s0, s1, s2, s3)\
		dst += dst_stride;		\
		LOAD_BYTE(s0, s1, 0)\
		src += src_stride;\
		add##_BYTE_ALIGN(s0, s1,s2,s3)\
		dst += dst_stride;\
        s2 = s3;\
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
}

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

///////////////////////////////////////
//for encoder

#define LoadDCT(c, d, dct)\
{					\
	c = *dct;		\
	d = *(dct+1);	\
}
#define Sub1stP(a, c, e)\
{					\
	e = a&0xFF;		\
	e = (e - c);	\
}
#define Sub2ndP(a, d, c)\
{					\
	c = ((a&0xFF00)>>8);\
	c = (c - d);		\
}
#define Store4Dct(a, dst, dct, c, d, e, f)\
{								\
	c = *((VO_U32*)(dst));	\
	d = a&0xff;					\
	e = c&0xff;					\
	e -= d; 					\
	d = ((a>>8)&0xff);			\
	f = ((c>>8)&0xff);			\
	f -= d;						\
	e = (e&0xffff)|(f<<16);		\
	*(dct++) = e;				\
	d = ((a>>16)&0xff);			\
	e = ((c>>16)&0xff);			\
	e -= d; 					\
	d = ((a>>24)&0xff);			\
	f = ((c>>24)&0xff);			\
	f -= d;						\
	e = (e&0xffff)|(f<<16);		\
	*(dct++) = e;				\
	*((VO_U32*)(dst)) =a;		\
}

// dct_data[p] = (Dst[p] - Src[p])
// Dst[p] = Src[p]
VO_VOID SubBlock(VO_U8 *src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch)
{
	VO_U8 *SrcEnd = src + 8*SrcPitch;
	VO_U32 a, b, c, d, e, f;
	VO_S32 *dct = (VO_S32 *)dct_data;

	do
	{
		LOAD_BYTE(a,b,0)//ref

		Store4Dct(a, Dst, dct, c, d, e, f)
		Store4Dct(b, Dst+4, dct, c, d, e, f)

		Dst += SrcPitch;
		src += SrcPitch;
	}
	while (src != SrcEnd);
}

// avg[p] = (Src[p] + Src[p+1] + 1) >> 1
// dct_data[p] = Dst[p] - avg[p]
// Dst[p] = avg[p]
VO_VOID SubBlockHor(VO_U8 *src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch)
{
	VO_U8 *SrcEnd = src + 8*SrcPitch;
	VO_U32 a,b,c,d,e,f;
	VO_S32 *dct = (VO_S32 *)dct_data;

	do
	{
		LOAD_BYTE(e,f,0)
		LOAD_BYTE(c,d,1)
		//Avg2(e,f,c,d,a,b)
        AVERAGE2_BYTE(e,f,c,d,a,b)
		Store4Dct(a, Dst, dct, c, d, e, f)
		Store4Dct(b, Dst+4, dct, c, d, e, f)

		Dst += SrcPitch;
		src += SrcPitch;
	}
	while (src != SrcEnd);
}

// avg[p] = (Src[p] + Src[p+SrcPitch] + 1) >> 1
// dct_data[p] = Dst[p] - avg[p]
// Dst[p] = avg[p]
VO_VOID SubBlockVer(VO_U8 *src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch)
{
	VO_U8 *SrcEnd = src + 8*SrcPitch;
	VO_U32 a,b,c,d,e,f,g,h;
	VO_S32 *dct = (VO_S32 *)dct_data;

	LOAD_BYTE(e,f,0)
	do
	{
		src += SrcPitch;
		LOAD_BYTE(c,d,0)
		//Avg2(e,f,c,d,a,b)
        AVERAGE2_BYTE(e,f,c,d,a,b)

		Store4Dct(a, Dst, dct, e, f, g, h)
		Store4Dct(b, Dst+4, dct, e, f, g, h)		

		e=c;
		f=d;
		Dst += SrcPitch;
	}
	while (src != SrcEnd);
}

// avg[p] = ((Src[p] + Src[p+1] + Src[p+SrcPitch] + Src[p+SrcPitch+1] + 2) >> 2
// dct_data[p] = Dst[p] - avg[p]
// Dst[p] = avg[p]
VO_VOID SubBlockHorVer(VO_U8 *src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch)
{
	VO_U8 *SrcEnd = src + 8*SrcPitch;
	VO_U32 a,b,c,d,g,h,i,j,k,l;
	VO_S32 *dct = (VO_S32 *)dct_data;

	LOAD_BYTE(a,b,0)
	LOAD_BYTE(c,d,1)

	//PrepareAvg4(a,b,c,d)
    PREPARE_AVERAGE4_BYTE(a,b,c,d)

	do
	{
		src += SrcPitch;
		LOAD_BYTE(g,h,0)
		LOAD_BYTE(i,j,1)

		//PrepareAvg4(g,h,i,j)
		//Avg4(a,b,c,d,g,h,i,j)

        PREPARE_AVERAGE4_BYTE(g,h,i,j)
        AVERAGE4_BYTE(a,b,c,d,g,h,i,j)

		Store4Dct(a, Dst, dct, c, d, k, l)
		Store4Dct(b, Dst+4, dct, c, d, k, l)
	
		a=g;  b=h;
		c=i;   d=j;
		Dst += SrcPitch;
	}
	while (src != SrcEnd);
}


// avgR[p] = (Src[p] + Src[p+1]) >> 1
// dct_data[p] = Dst[p] - avgR[p]
// Dst[p] = avgR[p]
VO_VOID SubBlockHorRound(VO_U8 *src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch)
{
	VO_U8 *SrcEnd = src + 8*SrcPitch;
	VO_U32 a,b,c,d,e,f;
	VO_S32 *dct = (VO_S32 *)dct_data;
	do
	{
		LOAD_BYTE(e,f,0)
		LOAD_BYTE(c,d,1)
		//Avg2Round(e,f,c,d,a,b)
        AVERAGE2_ROUND_BYTE(e,f,c,d,a,b)
		Store4Dct(a, Dst, dct, c, d, e, f)
		Store4Dct(b, Dst+4, dct, c, d, e, f)

		Dst += SrcPitch;
		src += SrcPitch;
	}
	while (src != SrcEnd);
}

// avgR[p] = (Src[p] + Src[p+SrcPitch]) >> 1
// dct_data[p] = Dst[p] - avgR[p]
// Dst[p] = avgR[p]
VO_VOID SubBlockVerRound(VO_U8 *src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch)
{
	VO_U8 *SrcEnd = src + 8*SrcPitch;
	VO_U32 a,b,c,d,e,f,g,h;
	VO_S32 *dct = (VO_S32 *)dct_data;
	LOAD_BYTE(e,f,0)
	do
	{
		src += SrcPitch;
		LOAD_BYTE(c,d,0)
		//Avg2Round(e,f,c,d,a,b)
        AVERAGE2_ROUND_BYTE(e,f,c,d,a,b)

		Store4Dct(a, Dst, dct, e, f, g, h)
		Store4Dct(b, Dst+4, dct, e, f, g, h)		

		e=c;
		f=d;
		Dst += SrcPitch;
	}
	while (src != SrcEnd);
}

// avgR[p] = ((Src[p] + Src[p+1] + Src[p+SrcPitch] + Src[p+SrcPitch+1] + 1) >> 2
// dct_data[p] = Dst[p] - avgR[p]
// Dst[p] = avgR[p]
VO_VOID SubBlockHorVerRound(VO_U8 *src, VO_S16* dct_data, VO_U8 *Dst, const VO_S32 SrcPitch)
{
	VO_U8 *SrcEnd = src + 8*SrcPitch;
	VO_U32 a,b,c,d,g,h,i,j,k,l;
	VO_S32 *dct = (VO_S32 *)dct_data;
	LOAD_BYTE(a,b,0)
	LOAD_BYTE(c,d,1)
	//PrepareAvg4(a,b,c,d)
    PREPARE_AVERAGE4_BYTE(a,b,c,d)
	do
	{
		src += SrcPitch;
		LOAD_BYTE(g,h,0)
		LOAD_BYTE(i,j,1)
		//PrepareAvg4(g,h,i,j)
	    //Avg4Round(a,b,c,d,g,h,i,j)

        PREPARE_AVERAGE4_BYTE(g,h,i,j)
        AVERAGE4_ROUND_BYTE(a,b,c,d,g,h,i,j)

       //printf(" a= %d ,b = %d \n",a,b);
		
		Store4Dct(a, Dst, dct, c, d, k, l)
		Store4Dct(b, Dst+4, dct, c, d, k, l)
		
		a=g;
		b=h;
		c=i;
		d=j;
		Dst += SrcPitch;
	}
	while (src != SrcEnd);
}