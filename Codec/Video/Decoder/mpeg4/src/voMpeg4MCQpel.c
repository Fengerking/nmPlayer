/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#include "voMpeg4MCQpel.h"


#define CLIP(s)\
	s >>= 5;\
	s = (s < 0 ? 0: (s > 255 ? 255: s));

#define QPIX0(s0, s1, s2, s3, s4, rd)				(14*s0 + 23*s1 - 7*s2 + 3*s3 - s4 + 16 - rd)
#define QPIX1(s0, s1, s2, s3, s4, s5, rd)			(19*s1 + 20*s2 - 6*s3 - s5 - 3*(s0-s4) + 16 - rd)
#define QPIX2(s0, s1, s2, s3, s4, s5, s6, rd)		(2*s0 - 6*(s1+s4) +20*(s2+s3) + 3*s5 - s6 + 16 - rd)
#define QPIX3(s0, s1, s2, s3, s4, s5, s6, s7, rd)	(3*(s1+s6)-6*(s2+s5) + 20*(s3+s4) - (s0+s7) + 16 - rd)


#define MCMPEG4_QPEL(name) \
	void MCMpeg4Block##name##_C(VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride, VO_S32 height, VO_S32 rd)\
{\
	do{\
	VO_S32 t;\
	t = QPIX0(src[SRC_PIX_POS*0], src[SRC_PIX_POS*1], src[SRC_PIX_POS*2], src[SRC_PIX_POS*3], src[SRC_PIX_POS*4], rd);\
	CLIP(t);\
	STORE_PIX(0, t);\
	t = QPIX1(src[SRC_PIX_POS*0], src[SRC_PIX_POS*1], src[SRC_PIX_POS*2], src[SRC_PIX_POS*3], src[SRC_PIX_POS*4], src[SRC_PIX_POS*5], rd);\
	CLIP(t);\
	STORE_PIX(1, t);\
	t = QPIX2(src[SRC_PIX_POS*0], src[SRC_PIX_POS*1], src[SRC_PIX_POS*2], src[SRC_PIX_POS*3], src[SRC_PIX_POS*4], src[SRC_PIX_POS*5], src[SRC_PIX_POS*6], rd);\
	CLIP(t);\
	STORE_PIX(2, t);\
	t = QPIX3(src[SRC_PIX_POS*0], src[SRC_PIX_POS*1], src[SRC_PIX_POS*2], src[SRC_PIX_POS*3], src[SRC_PIX_POS*4], src[SRC_PIX_POS*5], src[SRC_PIX_POS*6], src[SRC_PIX_POS*7], rd);\
	CLIP(t);\
	STORE_PIX(3, t);\
	t = QPIX3(src[SRC_PIX_POS*1], src[SRC_PIX_POS*2], src[SRC_PIX_POS*3], src[SRC_PIX_POS*4], src[SRC_PIX_POS*5], src[SRC_PIX_POS*6], src[SRC_PIX_POS*7], src[SRC_PIX_POS*8], rd);\
	CLIP(t);\
	STORE_PIX(4, t);\
	t = QPIX2(src[SRC_PIX_POS*8], src[SRC_PIX_POS*7], src[SRC_PIX_POS*6], src[SRC_PIX_POS*5], src[SRC_PIX_POS*4], src[SRC_PIX_POS*3], src[SRC_PIX_POS*2], rd);\
	CLIP(t);\
	STORE_PIX(5, t);\
	t = QPIX1(src[SRC_PIX_POS*8], src[SRC_PIX_POS*7], src[SRC_PIX_POS*6], src[SRC_PIX_POS*5], src[SRC_PIX_POS*4], src[SRC_PIX_POS*3], rd);\
	CLIP(t);\
	STORE_PIX(6, t);\
	t = QPIX0(src[SRC_PIX_POS*8], src[SRC_PIX_POS*7], src[SRC_PIX_POS*6], src[SRC_PIX_POS*5], src[SRC_PIX_POS*4], rd);\
	CLIP(t);\
	STORE_PIX(7, t);\
	dst += DST_STRIDE;\
	src += SRC_STRIDE;\
	}while(--height != 0);\
}

////////////////////////////////////////p frame
///////////////////////////////H
#define SRC_PIX_POS 1
#define SRC_STRIDE src_stride
#define DST_STRIDE dst_stride

#define STORE(d,s)  (d) = (VO_U8)(s)

#define STORE_PIX(i,t) \
	STORE(dst[i], t)

MCMPEG4_QPEL(Qpel2)

#undef STORE_PIX
#define STORE_PIX(i,t) \
	STORE(dst[i], (t+src[i]+1-rd) >> 1)

MCMPEG4_QPEL(Qpel1)

#undef STORE_PIX
#define STORE_PIX(i,t) \
	STORE(dst[i], (t+src[i+1]+1-rd) >> 1)

MCMPEG4_QPEL(Qpel3)

///////////////////////////////////V
#undef SRC_PIX_POS 
#undef SRC_STRIDE 
#undef DST_STRIDE 

#define SRC_PIX_POS	   src_stride
#define SRC_STRIDE 1
#define DST_STRIDE 1

#undef STORE_PIX
#define STORE_PIX(i,t) \
	STORE(dst[i*dst_stride], t)

MCMPEG4_QPEL(Qpel8)

#undef STORE_PIX
#define STORE_PIX(i,t) \
	STORE(dst[i*dst_stride], (t+src[i*src_stride]+1-rd) >> 1)

MCMPEG4_QPEL(Qpel4)

#undef STORE_PIX
#define STORE_PIX(i,t) \
	STORE(dst[dst_stride*i], (t+src[src_stride*i+src_stride]+1-rd) >> 1)

MCMPEG4_QPEL(Qpel12)

///////////////////////////////////B frame

#undef SRC_PIX_POS 
#undef SRC_STRIDE 
#undef DST_STRIDE 

#undef STORE
#undef STORE_PIX

#define SRC_PIX_POS 1
#define SRC_STRIDE src_stride
#define DST_STRIDE dst_stride

#define STORE(d, s)  (d) = (VO_U8)(((s)+(d)+1 ) >> 1)

#define STORE_PIX(i,t) \
	STORE(dst[i], t)

MCMPEG4_QPEL(Qpel2Add)

#undef STORE_PIX
#define STORE_PIX(i,t) \
	STORE(dst[i], (t+src[i]+1-rd) >> 1)

MCMPEG4_QPEL(Qpel1Add)

#undef STORE_PIX
#define STORE_PIX(i,t) \
	STORE(dst[i], (t+src[i+1]+1-rd) >> 1)

MCMPEG4_QPEL(Qpel3Add)

///////////////////////////////////V
#undef SRC_PIX_POS 
#undef SRC_STRIDE 
#undef DST_STRIDE 

#define SRC_PIX_POS	   src_stride
#define SRC_STRIDE 1
#define DST_STRIDE 1

#undef STORE_PIX
#define STORE_PIX(i,t) \
	STORE(dst[i*dst_stride], t)

MCMPEG4_QPEL(Qpel8Add)

#undef STORE_PIX
#define STORE_PIX(i,t) \
	STORE(dst[i*dst_stride], (t+src[i*src_stride]+1-rd) >> 1)

MCMPEG4_QPEL(Qpel4Add)

#undef STORE_PIX
#define STORE_PIX(i,t) \
	STORE(dst[dst_stride*i], (t+src[src_stride*i+src_stride]+1-rd) >> 1)

MCMPEG4_QPEL(Qpel12Add)

