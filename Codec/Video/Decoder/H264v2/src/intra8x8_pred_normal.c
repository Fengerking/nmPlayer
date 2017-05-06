#if 1
#include "global.h"
#include "intra8x8_pred.h"
//#include "image.h"

#define PREDICT8x8DC(dc)\
	VO_U32 y;\
	for(y = 8;y > 0;y--)\
	{\
	  M32(src) = M32(src+4) = dc;\
	  src+=stride;\
	}

#if defined(VOARMV6)
extern	void PredIntraLuma8x8V_ARMV6( VO_U8 *src , VO_S32 src_stride,VO_S32 block_available_up_left, VO_S32 block_available_up_right);	
extern	void PredIntraLuma8x8H_ARMV6( VO_U8 *src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right);  
extern     void Predict8x8VerLeft_ARMV6(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern    void Predict8x8DownLeft_ARMV6(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern	void Predict8x8VerRight_ARMV6( VO_U8 *src, VO_S32 stride, int topleft, int topright);  
extern     void Predict8x8DownRight_ARMV6(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern     void Predict8x8HorDown_ARMV6(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern	void Predict8x8HorUp_ARMV6( VO_U8 *src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right); 
extern	void Predict8x8DC_ARMV6( VO_U8 *src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right);  
extern	void Predict8x8DCLeft_ARMV6( VO_U8 *src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right);  
extern	void Predict8x8DCTop_ARMV6( VO_U8 *src , VO_S32 src_stride, VO_S32 block_available_up_left, VO_S32 block_available_up_right);  
extern   void Predict8x8DC128_ARMV6(VO_U8 *src, VO_S32 stride, int topleft, int topright);
#endif

#if defined(VOARMV7)
extern     void Predict8x8VerRight_ARMV7(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern     void Predict8x8DownRight_ARMV7(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern     void Predict8x8HorDown_ARMV7(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern     void Predict8x8HorUp_ARMV7(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern     void Predict8x8Ver_ARMV7(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern     void Predict8x8DownLeft_ARMV7(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern     void Predict8x8VerLeft_ARMV7(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern     void Predict8x8Hor_ARMV7(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern     void Predict8x8DC_ARMV7(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern     void Predict8x8DC128_ARMV7(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern     void Predict8x8DCLeft_ARMV7(VO_U8 *src, VO_S32 stride, int topleft, int topright);
extern     void Predict8x8DCTop_ARMV7(VO_U8 *src, VO_S32 stride, int topleft, int topright);
#endif

#if !defined(VOARMV6) && !defined(VOARMV7)

static void Predict8x8DC(VO_U8 *src, VO_S32 stride, int topleft, int topright)
{
  const VO_S32 l0 = ((topleft ? src[-1-stride] : src[-1]) + 2*src[-1] + src[-1+stride] + 2) >> 2;
  const VO_S32 l1 = (src[-1+(1-1)*stride] + 2*src[-1+1*stride] + src[-1+(1+1)*stride] + 2) >> 2;
  const VO_S32 l2 = (src[-1+(2-1)*stride] + 2*src[-1+2*stride] + src[-1+(2+1)*stride] + 2) >> 2;
  const VO_S32 l3 = (src[-1+(3-1)*stride] + 2*src[-1+3*stride] + src[-1+(3+1)*stride] + 2) >> 2;
  const VO_S32 l4 = (src[-1+(4-1)*stride] + 2*src[-1+4*stride] + src[-1+(4+1)*stride] + 2) >> 2;
  const VO_S32 l5 = (src[-1+(5-1)*stride] + 2*src[-1+5*stride] + src[-1+(5+1)*stride] + 2) >> 2;
  const VO_S32 l6 = (src[-1+(6-1)*stride] + 2*src[-1+6*stride] + src[-1+(6+1)*stride] + 2) >> 2;
  const VO_S32 l7 = (src[-1+6*stride] + 3*src[-1+7*stride] + 2) >> 2;
  VO_U8 *src1 = src-stride;
  const VO_S32 u0 = ((topleft ? src1[-1] : src1[0]) + 2*src1[0] + src1[1] + 2) >> 2;
  const VO_S32 u1 = (src1[0] + 2*src1[1] + src1[2] + 2) >> 2;
  const VO_S32 u2 = (src1[1] + 2*src1[2] + src1[3] + 2) >> 2;
  const VO_S32 u3 = (src1[2] + 2*src1[3] + src1[4] + 2) >> 2;
  const VO_S32 u4 = (src1[3] + 2*src1[4] + src1[5] + 2) >> 2;
  const VO_S32 u5 = (src1[4] + 2*src1[5] + src1[6] + 2) >> 2;
  const VO_S32 u6 = (src1[5] + 2*src1[6] + src1[7] + 2) >> 2;
  const VO_S32 u7 = ((topright ? src1[8] : src1[7]) + 2*src1[7] + src1[6] + 2) >> 2;
  const VO_U32 dc = ((l0+l1+l2+l3+l4+l5+l6+l7+u0+u1+u2+u3+u4+u5+u6+u7+8) >> 4) * 0x01010101;
  PREDICT8x8DC(dc);
}

static void Predict8x8DC128(VO_U8 *src, VO_S32 stride, int topleft, int topright)
{
  PREDICT8x8DC(0x80808080);
}

static void Predict8x8DCLeft(VO_U8 *src, VO_S32 stride, int topleft, int topright)
{
  const VO_S32 l0 = ((topleft ? src[-1-stride] : src[-1]) + 2*src[-1] + src[-1+stride] + 2) >> 2;
  const VO_S32 l1 = (src[-1+(1-1)*stride] + 2*src[-1+1*stride] + src[-1+(1+1)*stride] + 2) >> 2;
  const VO_S32 l2 = (src[-1+(2-1)*stride] + 2*src[-1+2*stride] + src[-1+(2+1)*stride] + 2) >> 2;
  const VO_S32 l3 = (src[-1+(3-1)*stride] + 2*src[-1+3*stride] + src[-1+(3+1)*stride] + 2) >> 2;
  const VO_S32 l4 = (src[-1+(4-1)*stride] + 2*src[-1+4*stride] + src[-1+(4+1)*stride] + 2) >> 2;
  const VO_S32 l5 = (src[-1+(5-1)*stride] + 2*src[-1+5*stride] + src[-1+(5+1)*stride] + 2) >> 2;
  const VO_S32 l6 = (src[-1+(6-1)*stride] + 2*src[-1+6*stride] + src[-1+(6+1)*stride] + 2) >> 2;
  const VO_S32 l7 = (src[-1+6*stride] + 3*src[-1+7*stride] + 2) >> 2;
  const VO_U32 dc = ((l0+l1+l2+l3+l4+l5+l6+l7+4) >> 3) * 0x01010101;
  PREDICT8x8DC(dc);
}

static void Predict8x8DCTop(VO_U8 *src, VO_S32 stride, int topleft, int topright)
{
  VO_U8 *src1 = src-stride;
  const VO_S32 u0 = ((topleft ? src1[-1] : src1[0]) + 2*src1[0] + src1[1] + 2) >> 2;
  const VO_S32 u1 = (src1[0] + 2*src1[1] + src1[2] + 2) >> 2;
  const VO_S32 u2 = (src1[1] + 2*src1[2] + src1[3] + 2) >> 2;
  const VO_S32 u3 = (src1[2] + 2*src1[3] + src1[4] + 2) >> 2;
  const VO_S32 u4 = (src1[3] + 2*src1[4] + src1[5] + 2) >> 2;
  const VO_S32 u5 = (src1[4] + 2*src1[5] + src1[6] + 2) >> 2;
  const VO_S32 u6 = (src1[5] + 2*src1[6] + src1[7] + 2) >> 2;
  const VO_S32 u7 = ((topright ? src1[8] : src1[7]) + 2*src1[7] + src1[6] + 2) >> 2;
  const VO_U32 dc = ((u0+u1+u2+u3+u4+u5+u6+u7+4) >> 3) * 0x01010101;
  PREDICT8x8DC(dc);
}

static void Predict8x8Hor_c(VO_U8 *src, VO_S32 stride, int topleft, int topright)
{
  const VO_S32 l0 = ((topleft ? src[-1-stride] : src[-1]) + 2*src[-1] + src[-1+stride] + 2) >> 2;
  const VO_S32 l1 = (src[-1+(1-1)*stride] + 2*src[-1+1*stride] + src[-1+(1+1)*stride] + 2) >> 2;
  const VO_S32 l2 = (src[-1+(2-1)*stride] + 2*src[-1+2*stride] + src[-1+(2+1)*stride] + 2) >> 2;
  const VO_S32 l3 = (src[-1+(3-1)*stride] + 2*src[-1+3*stride] + src[-1+(3+1)*stride] + 2) >> 2;
  const VO_S32 l4 = (src[-1+(4-1)*stride] + 2*src[-1+4*stride] + src[-1+(4+1)*stride] + 2) >> 2;
  const VO_S32 l5 = (src[-1+(5-1)*stride] + 2*src[-1+5*stride] + src[-1+(5+1)*stride] + 2) >> 2;
  const VO_S32 l6 = (src[-1+(6-1)*stride] + 2*src[-1+6*stride] + src[-1+(6+1)*stride] + 2) >> 2;
  const VO_S32 l7 = (src[-1+6*stride] + 3*src[-1+7*stride] + 2) >> 2;
  M32(src) = M32(src+4) = 0x01010101*l0;
  M32(src+stride) = M32(src+stride+4) = 0x01010101*l1;
  M32(src+2*stride) = M32(src+2*stride+4) = 0x01010101*l2;
  M32(src+3*stride) = M32(src+3*stride+4) = 0x01010101*l3;
  M32(src+4*stride) = M32(src+4*stride+4) = 0x01010101*l4;
  M32(src+5*stride) = M32(src+5*stride+4) = 0x01010101*l5;
  M32(src+6*stride) = M32(src+6*stride+4) = 0x01010101*l6;
  M32(src+7*stride) = M32(src+7*stride+4) = 0x01010101*l7;

}

static void Predict8x8Ver_c(VO_U8 *src, VO_S32 stride, int topleft, int topright)
{
  int j;
  VO_U8 *src1 = src-stride;
  const VO_S32 u0 = ((topleft ? src1[-1] : src1[0]) + 2*src1[0] + src1[1] + 2) >> 2;
  const VO_S32 u1 = (src1[0] + 2*src1[1] + src1[2] + 2) >> 2;
  const VO_S32 u2 = (src1[1] + 2*src1[2] + src1[3] + 2) >> 2;
  const VO_S32 u3 = (src1[2] + 2*src1[3] + src1[4] + 2) >> 2;
  const VO_S32 u4 = (src1[3] + 2*src1[4] + src1[5] + 2) >> 2;
  const VO_S32 u5 = (src1[4] + 2*src1[5] + src1[6] + 2) >> 2;
  const VO_S32 u6 = (src1[5] + 2*src1[6] + src1[7] + 2) >> 2;
  const VO_S32 u7 = ((topright ? src1[8] : src1[7]) + 2*src1[7] + src1[6] + 2) >> 2;
  src[0] = (VO_U8)u0;src[1] = (VO_U8)u1;src[2] = (VO_U8)u2;src[3] = (VO_U8)u3;
  src[4] = (VO_U8)u4;src[5] = (VO_U8)u5;src[6] = (VO_U8)u6;src[7] = (VO_U8)u7;
  for( j = 1; j < 8; j++ )
    M64(src+j*stride) = M64(src);
}

static void Predict8x8HorUp_c(VO_U8 *src, VO_S32 stride, int topleft, int topright)
{
  const VO_S32 l0 = ((topleft ? src[-1-stride] : src[-1]) + 2*src[-1] + src[-1+stride] + 2) >> 2;
  const VO_S32 l1 = (src[-1+(1-1)*stride] + 2*src[-1+1*stride] + src[-1+(1+1)*stride] + 2) >> 2;
  const VO_S32 l2 = (src[-1+(2-1)*stride] + 2*src[-1+2*stride] + src[-1+(2+1)*stride] + 2) >> 2;
  const VO_S32 l3 = (src[-1+(3-1)*stride] + 2*src[-1+3*stride] + src[-1+(3+1)*stride] + 2) >> 2;
  const VO_S32 l4 = (src[-1+(4-1)*stride] + 2*src[-1+4*stride] + src[-1+(4+1)*stride] + 2) >> 2;
  const VO_S32 l5 = (src[-1+(5-1)*stride] + 2*src[-1+5*stride] + src[-1+(5+1)*stride] + 2) >> 2;
  const VO_S32 l6 = (src[-1+(6-1)*stride] + 2*src[-1+6*stride] + src[-1+(6+1)*stride] + 2) >> 2;
  const VO_S32 l7 = (src[-1+6*stride] + 3*src[-1+7*stride] + 2) >> 2;
  src[0]= (VO_U8)((l0 + l1 + 1) >> 1);src[1]= (VO_U8)((l0 + 2*l1 + l2 + 2) >> 2);
  src[stride]=src[2]= (VO_U8)((l1 + l2 + 1) >> 1);
  src[1+stride]=src[3]= (VO_U8)((l1 + 2*l2 + l3 + 2) >> 2);
  src[2*stride]=src[2+stride]=src[4]= (VO_U8)((l2 + l3 + 1) >> 1);
  src[1+2*stride]=src[3+stride]=src[5]= (VO_U8)((l2 + 2*l3 + l4 + 2) >> 2);
  src[3*stride]=src[2+2*stride]=src[4+stride]=src[6]= (VO_U8)((l3 + l4 + 1) >> 1);
  src[1+3*stride]=src[3+2*stride]=src[5+stride]=src[7]= (VO_U8)((l3 + 2*l4 + l5 + 2) >> 2);
  src[4*stride]=src[2+3*stride]=src[4+2*stride]=src[6+stride]= (VO_U8)((l4 + l5 + 1) >> 1);
  src[1+4*stride]=src[3+3*stride]=src[5+2*stride]=src[7+stride]= (VO_U8)((l4 + 2*l5 + l6 + 2) >> 2);
  src[5*stride]=src[2+4*stride]=src[4+3*stride]=src[6+2*stride]= (VO_U8)((l5 + l6 + 1) >> 1);
  src[1+5*stride]=src[3+4*stride]=src[5+3*stride]=src[7+2*stride]= (VO_U8)((l5 + 2*l6 + l7 + 2) >> 2);
  src[6*stride]=src[2+5*stride]=src[4+4*stride]=src[6+3*stride]= (VO_U8)((l6 + l7 + 1) >> 1);
  src[1+6*stride]=src[3+5*stride]=src[5+4*stride]=src[7+3*stride]= (VO_U8)((l6 + 3*l7 + 2) >> 2);
  src[7*stride]=src[1+7*stride]=src[2+6*stride]=src[2+7*stride]=src[3+6*stride]=
  src[3+7*stride]=src[4+5*stride]=src[4+6*stride]=src[4+7*stride]=src[5+5*stride]=
  src[5+6*stride]=src[5+7*stride]=src[6+4*stride]=src[6+5*stride]=src[6+6*stride]=
  src[6+7*stride]=src[7+4*stride]=src[7+5*stride]=src[7+6*stride]=src[7+7*stride]= (VO_U8)l7;
}

static void Predict8x8HorDown_c(VO_U8 *src, VO_S32 stride, int topleft, int topright)
{
  const VO_S32 l0 = ((topleft ? src[-1-stride] : src[-1]) + 2*src[-1] + src[-1+stride] + 2) >> 2;
  const VO_S32 l1 = (src[-1+(1-1)*stride] + 2*src[-1+1*stride] + src[-1+(1+1)*stride] + 2) >> 2;
  const VO_S32 l2 = (src[-1+(2-1)*stride] + 2*src[-1+2*stride] + src[-1+(2+1)*stride] + 2) >> 2;
  const VO_S32 l3 = (src[-1+(3-1)*stride] + 2*src[-1+3*stride] + src[-1+(3+1)*stride] + 2) >> 2;
  const VO_S32 l4 = (src[-1+(4-1)*stride] + 2*src[-1+4*stride] + src[-1+(4+1)*stride] + 2) >> 2;
  const VO_S32 l5 = (src[-1+(5-1)*stride] + 2*src[-1+5*stride] + src[-1+(5+1)*stride] + 2) >> 2;
  const VO_S32 l6 = (src[-1+(6-1)*stride] + 2*src[-1+6*stride] + src[-1+(6+1)*stride] + 2) >> 2;
  const VO_S32 l7 = (src[-1+6*stride] + 3*src[-1+7*stride] + 2) >> 2;
  VO_U8 *src1 = src-stride;
  const VO_S32 u0 = ((topleft ? src1[-1] : src1[0]) + 2*src1[0] + src1[1] + 2) >> 2;
  const VO_S32 u1 = (src1[0] + 2*src1[1] + src1[2] + 2) >> 2;
  const VO_S32 u2 = (src1[1] + 2*src1[2] + src1[3] + 2) >> 2;
  const VO_S32 u3 = (src1[2] + 2*src1[3] + src1[4] + 2) >> 2;
  const VO_S32 u4 = (src1[3] + 2*src1[4] + src1[5] + 2) >> 2;
  const VO_S32 u5 = (src1[4] + 2*src1[5] + src1[6] + 2) >> 2;
  const VO_S32 u6 = (src1[5] + 2*src1[6] + src1[7] + 2) >> 2;
  const VO_S32 u7 = ((topright ? src1[8] : src1[7]) + 2*src1[7] + src1[6] + 2) >> 2;
  const VO_S32 lt = (src[-1] + 2*src1[-1] + src1[0] + 2) >> 2;
  src[0+7*stride]= (VO_U8)((l6 + l7 + 1) >> 1);
  src[1+7*stride]= (VO_U8)((l5 + 2*l6 + l7 + 2) >> 2);
  src[0+6*stride]=src[2+7*stride]= (VO_U8)((l5 + l6 + 1) >> 1);
  src[1+6*stride]=src[3+7*stride]= (VO_U8)((l4 + 2*l5 + l6 + 2) >> 2);
  src[0+5*stride]=src[2+6*stride]=src[4+7*stride]= (VO_U8)((l4 + l5 + 1) >> 1);
  src[1+5*stride]=src[3+6*stride]=src[5+7*stride]= (VO_U8)((l3 + 2*l4 + l5 + 2) >> 2);
  src[0+4*stride]=src[2+5*stride]=src[4+6*stride]=src[6+7*stride]= (VO_U8)((l3 + l4 + 1) >> 1);
  src[1+4*stride]=src[3+5*stride]=src[5+6*stride]=src[7+7*stride]= (VO_U8)((l2 + 2*l3 + l4 + 2) >> 2);
  src[0+3*stride]=src[2+4*stride]=src[4+5*stride]=src[6+6*stride]= (VO_U8)((l2 + l3 + 1) >> 1);
  src[1+3*stride]=src[3+4*stride]=src[5+5*stride]=src[7+6*stride]= (VO_U8)((l1 + 2*l2 + l3 + 2) >> 2);
  src[0+2*stride]=src[2+3*stride]=src[4+4*stride]=src[6+5*stride]= (VO_U8)((l1 + l2 + 1) >> 1);
  src[1+2*stride]=src[3+3*stride]=src[5+4*stride]=src[7+5*stride]= (VO_U8)((l0 + 2*l1 + l2 + 2) >> 2);
  src[0+1*stride]=src[2+2*stride]=src[4+3*stride]=src[6+4*stride]= (VO_U8)((l0 + l1 + 1) >> 1);
  src[1+1*stride]=src[3+2*stride]=src[5+3*stride]=src[7+4*stride]= (VO_U8)((lt + 2*l0 + l1 + 2) >> 2);
  src[0+0*stride]=src[2+1*stride]=src[4+2*stride]=src[6+3*stride]= (VO_U8)((lt + l0 + 1) >> 1);
  src[1+0*stride]=src[3+1*stride]=src[5+2*stride]=src[7+3*stride]= (VO_U8)((l0 + 2*lt + u0 + 2) >> 2);
  src[2+0*stride]=src[4+1*stride]=src[6+2*stride]= (VO_U8)((u1 + 2*u0 + lt + 2) >> 2);
  src[3+0*stride]=src[5+1*stride]=src[7+2*stride]= (VO_U8)((u2 + 2*u1 + u0 + 2) >> 2);
  src[4+0*stride]=src[6+1*stride]= (VO_U8)((u3 + 2*u2 + u1 + 2) >> 2);
  src[5+0*stride]=src[7+1*stride]= (VO_U8)((u4 + 2*u3 + u2 + 2) >> 2);
  src[6+0*stride]= (VO_U8)((u5 + 2*u4 + u3 + 2) >> 2);
  src[7+0*stride]= (VO_U8)((u6 + 2*u5 + u4 + 2) >> 2);
}

static void Predict8x8DownRight_c(VO_U8 *src, VO_S32 stride, int topleft, int topright)
{
  const VO_S32 l0 = ((topleft ? src[-1-stride] : src[-1]) + 2*src[-1] + src[-1+stride] + 2) >> 2;
  const VO_S32 l1 = (src[-1+(1-1)*stride] + 2*src[-1+1*stride] + src[-1+(1+1)*stride] + 2) >> 2;
  const VO_S32 l2 = (src[-1+(2-1)*stride] + 2*src[-1+2*stride] + src[-1+(2+1)*stride] + 2) >> 2;
  const VO_S32 l3 = (src[-1+(3-1)*stride] + 2*src[-1+3*stride] + src[-1+(3+1)*stride] + 2) >> 2;
  const VO_S32 l4 = (src[-1+(4-1)*stride] + 2*src[-1+4*stride] + src[-1+(4+1)*stride] + 2) >> 2;
  const VO_S32 l5 = (src[-1+(5-1)*stride] + 2*src[-1+5*stride] + src[-1+(5+1)*stride] + 2) >> 2;
  const VO_S32 l6 = (src[-1+(6-1)*stride] + 2*src[-1+6*stride] + src[-1+(6+1)*stride] + 2) >> 2;
  const VO_S32 l7 = (src[-1+6*stride] + 3*src[-1+7*stride] + 2) >> 2;
  VO_U8 *src1 = src-stride;
  const VO_S32 u0 = ((topleft ? src1[-1] : src1[0]) + 2*src1[0] + src1[1] + 2) >> 2;
  const VO_S32 u1 = (src1[0] + 2*src1[1] + src1[2] + 2) >> 2;
  const VO_S32 u2 = (src1[1] + 2*src1[2] + src1[3] + 2) >> 2;
  const VO_S32 u3 = (src1[2] + 2*src1[3] + src1[4] + 2) >> 2;
  const VO_S32 u4 = (src1[3] + 2*src1[4] + src1[5] + 2) >> 2;
  const VO_S32 u5 = (src1[4] + 2*src1[5] + src1[6] + 2) >> 2;
  const VO_S32 u6 = (src1[5] + 2*src1[6] + src1[7] + 2) >> 2;
  const VO_S32 u7 = ((topright ? src1[8] : src1[7]) + 2*src1[7] + src1[6] + 2) >> 2;
  const VO_S32 lt = (src[-1] + 2*src1[-1] + src1[0] + 2) >> 2;
  src[0+7*stride]= (VO_U8)((l7 + 2*l6 + l5 + 2) >> 2);
  src[0+6*stride]=src[1+7*stride]= (VO_U8)((l6 + 2*l5 + l4 + 2) >> 2);
  src[0+5*stride]=src[1+6*stride]=src[2+7*stride]= (VO_U8)((l5 + 2*l4 + l3 + 2) >> 2);
  src[0+4*stride]=src[1+5*stride]=src[2+6*stride]=src[3+7*stride]= (VO_U8)((l4 + 2*l3 + l2 + 2) >> 2);
  src[0+3*stride]=src[1+4*stride]=src[2+5*stride]=src[3+6*stride]=src[4+7*stride]= (VO_U8)((l3 + 2*l2 + l1 + 2) >> 2);
  src[0+2*stride]=src[1+3*stride]=src[2+4*stride]=src[3+5*stride]=src[4+6*stride]=src[5+7*stride]= (VO_U8)((l2 + 2*l1 + l0 + 2) >> 2);
  src[0+1*stride]=src[1+2*stride]=src[2+3*stride]=src[3+4*stride]=src[4+5*stride]=src[5+6*stride]=src[6+7*stride]= (VO_U8)((l1 + 2*l0 + lt + 2) >> 2);
  src[0+0*stride]=src[1+1*stride]=src[2+2*stride]=src[3+3*stride]=src[4+4*stride]=src[5+5*stride]=src[6+6*stride]=src[7+7*stride]= (VO_U8)((l0 + 2*lt + u0 + 2) >> 2);
  src[1+0*stride]=src[2+1*stride]=src[3+2*stride]=src[4+3*stride]=src[5+4*stride]=src[6+5*stride]=src[7+6*stride]= (VO_U8)((lt + 2*u0 + u1 + 2) >> 2);
  src[2+0*stride]=src[3+1*stride]=src[4+2*stride]=src[5+3*stride]=src[6+4*stride]=src[7+5*stride]= (VO_U8)((u0 + 2*u1 + u2 + 2) >> 2);
  src[3+0*stride]=src[4+1*stride]=src[5+2*stride]=src[6+3*stride]=src[7+4*stride]= (VO_U8)((u1 + 2*u2 + u3 + 2) >> 2);
  src[4+0*stride]=src[5+1*stride]=src[6+2*stride]=src[7+3*stride]= (VO_U8)((u2 + 2*u3 + u4 + 2) >> 2);
  src[5+0*stride]=src[6+1*stride]=src[7+2*stride]= (VO_U8)((u3 + 2*u4 + u5 + 2) >> 2);
  src[6+0*stride]=src[7+1*stride]= (VO_U8)((u4 + 2*u5 + u6 + 2) >> 2);
  src[7+0*stride]= (VO_U8)((u5 + 2*u6 + u7 + 2) >> 2);

}

static void Predict8x8VerRight_c(VO_U8 *src, VO_S32 stride, int topleft, int topright)
{
  const VO_S32 l0 = ((topleft ? src[-1-stride] : src[-1]) + 2*src[-1] + src[-1+stride] + 2) >> 2;
  const VO_S32 l1 = (src[-1+(1-1)*stride] + 2*src[-1+1*stride] + src[-1+(1+1)*stride] + 2) >> 2;
  const VO_S32 l2 = (src[-1+(2-1)*stride] + 2*src[-1+2*stride] + src[-1+(2+1)*stride] + 2) >> 2;
  const VO_S32 l3 = (src[-1+(3-1)*stride] + 2*src[-1+3*stride] + src[-1+(3+1)*stride] + 2) >> 2;
  const VO_S32 l4 = (src[-1+(4-1)*stride] + 2*src[-1+4*stride] + src[-1+(4+1)*stride] + 2) >> 2;
  const VO_S32 l5 = (src[-1+(5-1)*stride] + 2*src[-1+5*stride] + src[-1+(5+1)*stride] + 2) >> 2;
  const VO_S32 l6 = (src[-1+(6-1)*stride] + 2*src[-1+6*stride] + src[-1+(6+1)*stride] + 2) >> 2;
  const VO_S32 l7 = (src[-1+6*stride] + 3*src[-1+7*stride] + 2) >> 2;
  VO_U8 *src1 = src-stride;
  const VO_S32 u0 = ((topleft ? src1[-1] : src1[0]) + 2*src1[0] + src1[1] + 2) >> 2;
  const VO_S32 u1 = (src1[0] + 2*src1[1] + src1[2] + 2) >> 2;
  const VO_S32 u2 = (src1[1] + 2*src1[2] + src1[3] + 2) >> 2;
  const VO_S32 u3 = (src1[2] + 2*src1[3] + src1[4] + 2) >> 2;
  const VO_S32 u4 = (src1[3] + 2*src1[4] + src1[5] + 2) >> 2;
  const VO_S32 u5 = (src1[4] + 2*src1[5] + src1[6] + 2) >> 2;
  const VO_S32 u6 = (src1[5] + 2*src1[6] + src1[7] + 2) >> 2;
  const VO_S32 u7 = ((topright ? src1[8] : src1[7]) + 2*src1[7] + src1[6] + 2) >> 2;
  const VO_S32 lt = (src[-1] + 2*src1[-1] + src1[0] + 2) >> 2;
  src[0+6*stride]= (VO_U8)((l5 + 2*l4 + l3 + 2) >> 2);
  src[0+7*stride]= (VO_U8)((l6 + 2*l5 + l4 + 2) >> 2);
  src[0+4*stride]=src[1+6*stride]= (VO_U8)((l3 + 2*l2 + l1 + 2) >> 2);
  src[0+5*stride]=src[1+7*stride]= (VO_U8)((l4 + 2*l3 + l2 + 2) >> 2);
  src[0+2*stride]=src[1+4*stride]=src[2+6*stride]= (VO_U8)((l1 + 2*l0 + lt + 2) >> 2);
  src[0+3*stride]=src[1+5*stride]=src[2+7*stride]= (VO_U8)((l2 + 2*l1 + l0 + 2) >> 2);
  src[0+1*stride]=src[1+3*stride]=src[2+5*stride]=src[3+7*stride]= (VO_U8)((l0 + 2*lt + u0 + 2) >> 2);
  src[0+0*stride]=src[1+2*stride]=src[2+4*stride]=src[3+6*stride]= (VO_U8)((lt + u0 + 1) >> 1);
  src[1+1*stride]=src[2+3*stride]=src[3+5*stride]=src[4+7*stride]= (VO_U8)((lt + 2*u0 + u1 + 2) >> 2);
  src[1+0*stride]=src[2+2*stride]=src[3+4*stride]=src[4+6*stride]= (VO_U8)((u0 + u1 + 1) >> 1);
  src[2+1*stride]=src[3+3*stride]=src[4+5*stride]=src[5+7*stride]= (VO_U8)((u0 + 2*u1 + u2 + 2) >> 2);
  src[2+0*stride]=src[3+2*stride]=src[4+4*stride]=src[5+6*stride]= (VO_U8)((u1 + u2 + 1) >> 1);
  src[3+1*stride]=src[4+3*stride]=src[5+5*stride]=src[6+7*stride]= (VO_U8)((u1 + 2*u2 + u3 + 2) >> 2);
  src[3+0*stride]=src[4+2*stride]=src[5+4*stride]=src[6+6*stride]= (VO_U8)((u2 + u3 + 1) >> 1);
  src[4+1*stride]=src[5+3*stride]=src[6+5*stride]=src[7+7*stride]= (VO_U8)((u2 + 2*u3 + u4 + 2) >> 2);
  src[4+0*stride]=src[5+2*stride]=src[6+4*stride]=src[7+6*stride]= (VO_U8)((u3 + u4 + 1) >> 1);
  src[5+1*stride]=src[6+3*stride]=src[7+5*stride]= (VO_U8)((u3 + 2*u4 + u5 + 2) >> 2);
  src[5+0*stride]=src[6+2*stride]=src[7+4*stride]= (VO_U8)((u4 + u5 + 1) >> 1);
  src[6+1*stride]=src[7+3*stride]= (VO_U8)((u4 + 2*u5 + u6 + 2) >> 2);
  src[6+0*stride]=src[7+2*stride]= (VO_U8)((u5 + u6 + 1) >> 1);
  src[7+1*stride]= (VO_U8)((u5 + 2*u6 + u7 + 2) >> 2);
  src[7+0*stride]= (VO_U8)((u6 + u7 + 1) >> 1);
}

static void Predict8x8VerLeft_c(VO_U8 *src, VO_S32 stride, int topleft, int topright)
{
  VO_U8 *src1 = src-stride;
  const VO_S32 u0 = ((topleft ? src1[-1] : src1[0]) + 2*src1[0] + src1[1] + 2) >> 2;
  const VO_S32 u1 = (src1[0] + 2*src1[1] + src1[2] + 2) >> 2;
  const VO_S32 u2 = (src1[1] + 2*src1[2] + src1[3] + 2) >> 2;
  const VO_S32 u3 = (src1[2] + 2*src1[3] + src1[4] + 2) >> 2;
  const VO_S32 u4 = (src1[3] + 2*src1[4] + src1[5] + 2) >> 2;
  const VO_S32 u5 = (src1[4] + 2*src1[5] + src1[6] + 2) >> 2;
  const VO_S32 u6 = (src1[5] + 2*src1[6] + src1[7] + 2) >> 2;
  const VO_S32 u7 = ((topright ? src1[8] : src1[7]) + 2*src1[7] + src1[6] + 2) >> 2;
  VO_S32 u8, u9, u10, u11, u12, u13, u14, u15; 
  if(topright) 
  { 
    u8  = (src1[7] + 2*src1[8] + src1[9] + 2) >> 2;
	u9  = (src1[8] + 2*src1[9] + src1[10] + 2) >> 2;
	u10 = (src1[9] + 2*src1[10] + src1[11] + 2) >> 2;
	u11 = (src1[10] + 2*src1[11] + src1[12] + 2) >> 2;
	u12 = (src1[11] + 2*src1[12] + src1[13] + 2) >> 2;
	u13 = (src1[12] + 2*src1[13] + src1[14] + 2) >> 2;
	u14 = (src1[13] + 2*src1[14] + src1[15] + 2) >> 2;
    u15 = (src1[14] + 3*src1[15] + 2) >> 2; 
  }
  else
  {
  	u8=u9=u10=u11=u12=u13=u14=u15= src1[7];
  }
  src[0+0*stride]= (VO_U8)((u0 + u1 + 1) >> 1);
  src[0+1*stride]= (VO_U8)((u0 + 2*u1 + u2 + 2) >> 2);
  src[0+2*stride]=src[1+0*stride]= (VO_U8)((u1 + u2 + 1) >> 1);
  src[0+3*stride]=src[1+1*stride]= (VO_U8)((u1 + 2*u2 + u3 + 2) >> 2);
  src[0+4*stride]=src[1+2*stride]=src[2+0*stride]= (VO_U8)((u2 + u3 + 1) >> 1);
  src[0+5*stride]=src[1+3*stride]=src[2+1*stride]= (VO_U8)((u2 + 2*u3 + u4 + 2) >> 2);
  src[0+6*stride]=src[1+4*stride]=src[2+2*stride]=src[3+0*stride]= (VO_U8)((u3 + u4 + 1) >> 1);
  src[0+7*stride]=src[1+5*stride]=src[2+3*stride]=src[3+1*stride]= (VO_U8)((u3 + 2*u4 + u5 + 2) >> 2);
  src[1+6*stride]=src[2+4*stride]=src[3+2*stride]=src[4+0*stride]= (VO_U8)((u4 + u5 + 1) >> 1);
  src[1+7*stride]=src[2+5*stride]=src[3+3*stride]=src[4+1*stride]= (VO_U8)((u4 + 2*u5 + u6 + 2) >> 2);
  src[2+6*stride]=src[3+4*stride]=src[4+2*stride]=src[5+0*stride]= (VO_U8)((u5 + u6 + 1) >> 1);
  src[2+7*stride]=src[3+5*stride]=src[4+3*stride]=src[5+1*stride]= (VO_U8)((u5 + 2*u6 + u7 + 2) >> 2);
  src[3+6*stride]=src[4+4*stride]=src[5+2*stride]=src[6+0*stride]= (VO_U8)((u6 + u7 + 1) >> 1);
  src[3+7*stride]=src[4+5*stride]=src[5+3*stride]=src[6+1*stride]= (VO_U8)((u6 + 2*u7 + u8 + 2) >> 2);
  src[4+6*stride]=src[5+4*stride]=src[6+2*stride]=src[7+0*stride]= (VO_U8)((u7 + u8 + 1) >> 1);
  src[4+7*stride]=src[5+5*stride]=src[6+3*stride]=src[7+1*stride]= (VO_U8)((u7 + 2*u8 + u9 + 2) >> 2);
  src[5+6*stride]=src[6+4*stride]=src[7+2*stride]= (VO_U8)((u8 + u9 + 1) >> 1);
  src[5+7*stride]=src[6+5*stride]=src[7+3*stride]= (VO_U8)((u8 + 2*u9 + u10 + 2) >> 2);
  src[6+6*stride]=src[7+4*stride]= (VO_U8)((u9 + u10 + 1) >> 1);
  src[6+7*stride]=src[7+5*stride]= (VO_U8)((u9 + 2*u10 + u11 + 2) >> 2);
  src[7+6*stride]= (VO_U8)((u10 + u11 + 1) >> 1);
  src[7+7*stride]= (VO_U8)((u10 + 2*u11 + u12 + 2) >> 2);
}

static void Predict8x8DownLeft_c(VO_U8 *src, VO_S32 stride, int topleft, int topright)
{
  VO_U8 *src1 = src-stride;
  const VO_S32 u0 = ((topleft ? src1[-1] : src1[0]) + 2*src1[0] + src1[1] + 2) >> 2;
  const VO_S32 u1 = (src1[0] + 2*src1[1] + src1[2] + 2) >> 2;
  const VO_S32 u2 = (src1[1] + 2*src1[2] + src1[3] + 2) >> 2;
  const VO_S32 u3 = (src1[2] + 2*src1[3] + src1[4] + 2) >> 2;
  const VO_S32 u4 = (src1[3] + 2*src1[4] + src1[5] + 2) >> 2;
  const VO_S32 u5 = (src1[4] + 2*src1[5] + src1[6] + 2) >> 2;
  const VO_S32 u6 = (src1[5] + 2*src1[6] + src1[7] + 2) >> 2;
  const VO_S32 u7 = ((topright ? src1[8] : src1[7]) + 2*src1[7] + src1[6] + 2) >> 2;
  VO_S32 u8, u9, u10, u11, u12, u13, u14, u15; 
  if(topright) 
  { 
    u8  = (src1[7] + 2*src1[8] + src1[9] + 2) >> 2;
	u9  = (src1[8] + 2*src1[9] + src1[10] + 2) >> 2;
	u10 = (src1[9] + 2*src1[10] + src1[11] + 2) >> 2;
	u11 = (src1[10] + 2*src1[11] + src1[12] + 2) >> 2;
	u12 = (src1[11] + 2*src1[12] + src1[13] + 2) >> 2;
	u13 = (src1[12] + 2*src1[13] + src1[14] + 2) >> 2;
	u14 = (src1[13] + 2*src1[14] + src1[15] + 2) >> 2;
    u15 = (src1[14] + 3*src1[15] + 2) >> 2; 
  }
  else
  {
  	u8=u9=u10=u11=u12=u13=u14=u15= src1[7];
  }
  src[0+0*stride]= (VO_U8)((u0 + 2*u1 + u2 + 2) >> 2);
  src[0+1*stride]=src[1+0*stride]= (VO_U8)((u1 + 2*u2 + u3 + 2) >> 2);
  src[0+2*stride]=src[1+1*stride]=src[2+0*stride]= (VO_U8)((u2 + 2*u3 + u4 + 2) >> 2);
  src[0+3*stride]=src[1+2*stride]=src[2+1*stride]=src[3+0*stride]= (VO_U8)((u3 + 2*u4 + u5 + 2) >> 2);
  src[0+4*stride]=src[1+3*stride]=src[2+2*stride]=src[3+1*stride]=src[4+0*stride]= (VO_U8)((u4 + 2*u5 + u6 + 2) >> 2);
  src[0+5*stride]=src[1+4*stride]=src[2+3*stride]=src[3+2*stride]=src[4+1*stride]=src[5+0*stride]= (VO_U8)((u5 + 2*u6 + u7 + 2) >> 2);
  src[0+6*stride]=src[1+5*stride]=src[2+4*stride]=src[3+3*stride]=src[4+2*stride]=src[5+1*stride]=src[6+0*stride]= (VO_U8)((u6 + 2*u7 + u8 + 2) >> 2);
  src[0+7*stride]=src[1+6*stride]=src[2+5*stride]=src[3+4*stride]=src[4+3*stride]=src[5+2*stride]=src[6+1*stride]=src[7+0*stride]= (VO_U8)((u7 + 2*u8 + u9 + 2) >> 2);
  src[1+7*stride]=src[2+6*stride]=src[3+5*stride]=src[4+4*stride]=src[5+3*stride]=src[6+2*stride]=src[7+1*stride]= (VO_U8)((u8 + 2*u9 + u10 + 2) >> 2);
  src[2+7*stride]=src[3+6*stride]=src[4+5*stride]=src[5+4*stride]=src[6+3*stride]=src[7+2*stride]= (VO_U8)((u9 + 2*u10 + u11 + 2) >> 2);
  src[3+7*stride]=src[4+6*stride]=src[5+5*stride]=src[6+4*stride]=src[7+3*stride]= (VO_U8)((u10 + 2*u11 + u12 + 2) >> 2);
  src[4+7*stride]=src[5+6*stride]=src[6+5*stride]=src[7+4*stride]= (VO_U8)((u11 + 2*u12 + u13 + 2) >> 2);
  src[5+7*stride]=src[6+6*stride]=src[7+5*stride]= (VO_U8)((u12 + 2*u13 + u14 + 2) >> 2);
  src[6+7*stride]=src[7+6*stride]= (VO_U8)((u13 + 2*u14 + u15 + 2) >> 2);
  src[7+7*stride]= (VO_U8)((u14 + 3*u15 + 2) >> 2);
}
#endif

int intrapred8x8_normal(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int ioff, int joff)              //!< joff

{  
// #if USE_MULTI_THREAD
// 	VO_S8 predmode = pDecGlobal->ipredmode_frame[pDecLocal->mbAddrX*16 + joff + (ioff>>2)];
// #else
	VO_S8 predmode = pDecLocal->ipredmode[12+(joff<<1)+(ioff>>2)];
// #endif
  Slice *currSlice = pDecLocal->p_Slice;
  VO_S32 iLumaStride = pDecGlobal->iLumaStride<<pDecLocal->mb_field;
  imgpel *imgY = pDecGlobal->dec_picture->imgY;

  VO_U8 *pSrc = imgY+(pDecLocal->pix_y + joff)*iLumaStride+pDecLocal->pix_x + ioff;  
  int block_available_up_right;
  int block_available_up_left;
  int i = (ioff>>1)+joff;
  if(pDecLocal->mb_field)
  {
    pSrc = imgY+(pDecLocal->pix_y )*pDecGlobal->iLumaStride+pDecLocal->pix_x + ioff;
	//if(pDecLocal->mb.y&1)
	//  pSrc -= 15*pDecGlobal->iLumaStride;
	pSrc += joff*iLumaStride;
  }

  block_available_up_left = (pDecLocal->intra_ava_topleft<<i)&0x8000?1:0;
  block_available_up_right = (pDecLocal->intra_ava_topright<<i)&0x4000?1:0;
  
  switch (predmode)
  {
  case DC_PRED:
#if defined(VOARMV7)
    Predict8x8DC_ARMV7(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#elif defined(VOARMV6)
    Predict8x8DC_ARMV6(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#else
    Predict8x8DC(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#endif
	return DECODING_OK;
    break;
  case DC_128_PRED:
#if defined(VOARMV7)
    Predict8x8DC128_ARMV7(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#elif defined(VOARMV6)
    Predict8x8DC128_ARMV6(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#else 	
    Predict8x8DC128(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#endif
	return DECODING_OK;
    break;
  case DC_LEFT_PRED:
 #if defined(VOARMV7)
    Predict8x8DCLeft_ARMV7(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#elif defined(VOARMV6)
    Predict8x8DCLeft_ARMV6(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#else
    Predict8x8DCLeft(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#endif
	return DECODING_OK;
    break;
  case DC_TOP_PRED:
 #if defined(VOARMV7)
    Predict8x8DCTop_ARMV7(pSrc,iLumaStride, block_available_up_left, block_available_up_right);	
#elif defined(VOARMV6)  	
    Predict8x8DCTop_ARMV6(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#else
    Predict8x8DCTop(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#endif
	return DECODING_OK;
    break;
  case HOR_PRED:
#if defined(VOARMV7) 
    Predict8x8Hor_ARMV7(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#elif defined(VOARMV6)
    PredIntraLuma8x8H_ARMV6(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#else  	
    Predict8x8Hor_c(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#endif
	return DECODING_OK;
    break;
  case VERT_PRED:
#if defined(VOARMV7) 
     Predict8x8Ver_ARMV7(pSrc,iLumaStride, block_available_up_left, block_available_up_right); 	
#elif defined(VOARMV6)
    PredIntraLuma8x8V_ARMV6(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#else
    Predict8x8Ver_c(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#endif
	return DECODING_OK;
    break;
  case HOR_UP_PRED:
#if defined(VOARMV7) 
    Predict8x8HorUp_ARMV7(pSrc,iLumaStride, block_available_up_left, block_available_up_right);	
#elif defined(VOARMV6)
    Predict8x8HorUp_ARMV6(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#else
    Predict8x8HorUp_c(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#endif
	return DECODING_OK;
    break;
  case HOR_DOWN_PRED:
#if defined(VOARMV7) 
    Predict8x8HorDown_ARMV7(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#elif defined(VOARMV6)
    Predict8x8HorDown_ARMV6(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#else
    Predict8x8HorDown_c(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#endif
	return DECODING_OK;
    break;
  case DIAG_DOWN_RIGHT_PRED:
#if defined(VOARMV7)
     Predict8x8DownRight_ARMV7(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#elif defined(VOARMV6)
     Predict8x8DownRight_ARMV6(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#else
    Predict8x8DownRight_c(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#endif
	return DECODING_OK;
    break;
  case DIAG_DOWN_LEFT_PRED:
#if defined(VOARMV7)  
    Predict8x8DownLeft_ARMV7(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#elif defined(VOARMV6)
    Predict8x8DownLeft_ARMV6(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#else
    Predict8x8DownLeft_c(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#endif
	return DECODING_OK;
    break;
  case VERT_RIGHT_PRED:
#if defined(VOARMV7) 
    Predict8x8VerRight_ARMV7(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#elif defined(VOARMV6)
    Predict8x8VerRight_ARMV6(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#else
    Predict8x8VerRight_c(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#endif
	return DECODING_OK;
    break;
  case VERT_LEFT_PRED:
#if defined(VOARMV7)   
     Predict8x8VerLeft_ARMV7(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#elif defined(VOARMV6)
    Predict8x8VerLeft_ARMV6(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#else
    Predict8x8VerLeft_c(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#endif
	return DECODING_OK;
    break;
  default:
#if defined(VOARMV7)
	Predict8x8DC128_ARMV7(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#elif defined(VOARMV6)
	Predict8x8DC128_ARMV6(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#else 	
	Predict8x8DC128(pSrc,iLumaStride, block_available_up_left, block_available_up_right);
#endif
	return DECODING_OK;
    break;
  }
}
#endif

