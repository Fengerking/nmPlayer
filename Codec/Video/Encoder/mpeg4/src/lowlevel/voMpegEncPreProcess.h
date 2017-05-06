/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _PREPROCESS_H_
#define _PREPROCESS_H_
#include "../voMpegProtect.h"
#include "voType.h"

typedef VO_VOID (YUV420_TO_YUV420)(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
								VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
								VO_U8 * y_src, VO_U8 * u_src, VO_U8 * v_src,
								VO_S32 y_src_stride, VO_S32 uv_src_stride,
								VO_S32 width, VO_S32 height);

typedef YUV420_TO_YUV420 * YUV420_TO_YUV420_PTR;

typedef VO_VOID (IMG_TO_YUV420)(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
							 VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
							 VO_U8 * src, VO_S32 width, VO_S32 height);


typedef IMG_TO_YUV420 * IMG_TO_YUV420_PTR;

typedef VO_VOID (YUV420PACK_TO_YUV420)(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
								VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
								VO_U8 * y_src, VO_U8 * uv_src,
								VO_S32 y_src_stride, VO_S32 uv_src_stride,
								VO_S32 width, VO_S32 height);

typedef YUV420PACK_TO_YUV420 * YUV420PACK_TO_YUV420_PTR;


extern VO_VOID YUV420toYUV420_C(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
				   VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
				   VO_U8 * y_src, VO_U8 * u_src, VO_U8 * v_src,
				   VO_S32 y_src_stride, VO_S32 uv_src_stride,
				   VO_S32 width, VO_S32 height);

extern VO_VOID YVYtoYUV420_C(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
				 VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
				 VO_U8 * uyvy_src, VO_S32 width, VO_S32 height);

extern VO_VOID YUYVtoYUV420_C(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
				 VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
				 VO_U8 * uyvy_src, VO_S32 width, VO_S32 height);

extern VO_VOID UYVYtoYUV420RT180_C(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
				 VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
				 VO_U8 * uyvy_src, VO_S32 width, VO_S32 height);

extern VO_VOID RGB565toYUV420Disable_C(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
				   VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
				   VO_U8 * rgb565_src, VO_S32 width, VO_S32 height);

extern VO_VOID RGB565toYUV420FilpY_C(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
						 VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
						 VO_U8 * rgb565_src, VO_S32 width, VO_S32 height);

extern VO_VOID RGB565toYUV420RT180_C(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
						 VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
						 VO_U8 * rgb565_src, VO_S32 width, VO_S32 height);

extern VO_VOID RGB565toYV12RT90L_C(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
				VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
				VO_U8 * rgb565_src, VO_S32 width, VO_S32 height);

extern VO_VOID VYUY2toYUV420Disable_C(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
						  VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
						  VO_U8 * uyvy_src,	VO_S32 width, VO_S32 height);

extern VO_VOID VYUY2toYUV420RT180_C(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
						VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
						VO_U8 * uyvy_src,	VO_S32 width, VO_S32 height);

extern VO_VOID YUV420PacktoYUV420_C(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
				   VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
				   VO_U8 * y_src, VO_U8 * uv_src,
				   VO_S32 y_src_stride, VO_S32 uv_src_stride,
				   VO_S32 width, VO_S32 height);

extern VO_VOID RGB32toYUV420Disable_C(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
						   VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
						   VO_U8 * rgb32_src, VO_S32 width, VO_S32 height);

YUV420_TO_YUV420 YUV420toYUV420_C;
IMG_TO_YUV420 UYVYtoYUV420_C;
IMG_TO_YUV420 UYVYtoYUV420RT180_C;
IMG_TO_YUV420 YUYVtoYUV420_C;
IMG_TO_YUV420 YUYVtoYUV420RT180_C;
IMG_TO_YUV420 RGB565toYUV420Disable_C;
IMG_TO_YUV420 RGB565toYUV420FilpY_C;
IMG_TO_YUV420 RGB565toYUV420RT180_C;
IMG_TO_YUV420 RGB565toYV12RT90L_C;
IMG_TO_YUV420 VYUY2toYUV420Disable_C;
IMG_TO_YUV420 VYUY2toYUV420RT180_C;

IMG_TO_YUV420 RGB32toYUV420Disable_C;

YUV420PACK_TO_YUV420 YUV420PacktoYUV420_C; 

extern const YUV420_TO_YUV420_PTR yuv420_to_yuv420_all[6];
extern const IMG_TO_YUV420_PTR uyvy_to_yuv420_all[6];
extern const IMG_TO_YUV420_PTR rgb565_to_yuv420_all[6];
extern const IMG_TO_YUV420_PTR rgb32_to_yuv420_all[6];
extern const IMG_TO_YUV420_PTR vyuy2_to_yuv420_all[6];
extern const IMG_TO_YUV420_PTR yuyv_to_yuv420_all[6];
extern const YUV420PACK_TO_YUV420_PTR yuv420_pack_2_all[6];



#if defined (VOWMMX)
#define uyvy_to_yuv420 UYVYorVYUYtoPlanar420_WMMX
#define rgb565_to_yuv420 RGB565toYUV420Disable_C
#define uyvy_to_yuv420_rt180 UYVYtoYUV420RT180_C
#define rgb32_to_yuv420 RGB32toYUV420Disable_C
#elif (defined (VOARMV4) || defined(VOARMV6))
#define uyvy_to_yuv420           UYVYtoYUV420_C
#define uyvy_to_yuv420_rt180   UYVYtoYUV420RT180_C
#define rgb565_to_yuv420         RGB565toYUV420Disable_C
#define rgb32_to_yuv420 RGB32toYUV420Disable_C
#else
#define uyvy_to_yuv420 UYVYtoYUV420_C
#define uyvy_to_yuv420_rt180 UYVYtoYUV420RT180_C
#define rgb565_to_yuv420 RGB565toYUV420Disable_C
#define rgb32_to_yuv420 RGB32toYUV420Disable_C
#endif

#define RDITHER_P0		0
#define RDITHER_P1		0
#define RDITHER_P2		0
#define RDITHER_P3		0

#define GDITHER_P0		0
#define GDITHER_P1		0
#define GDITHER_P2		0
#define GDITHER_P3		0

#define BDITHER_P0		0
#define BDITHER_P1		0
#define BDITHER_P2		0
#define BDITHER_P3		0



#endif //_PREPROCESS_H_
