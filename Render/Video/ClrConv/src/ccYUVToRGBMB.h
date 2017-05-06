/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _YUV_TO_RGB_MB_H_
#define _YUV_TO_RGB_MB_H_

#include "voYYDef_CC.h"
#include "voColorConversion.h"
#include "ccInternal.h"

CC_MB cc_mb;
CC_MB cc_mb_new;
CC_MB cc_rgb32_mb;
CC_MB cc_rgb32_mb_new;
CC_MB cc_argb32_mb;
CC_MB cc_argb32_mb_new;
CC_MB yuv420_rgb24;
CC_MB_NEW YUV444_to_RGB565_cc_mb;
CC_MB_NEW YUV422_21_to_RGB565_cc_mb;
CC_MB_NEW YUV411_to_RGB565_cc_mb;
CC_MB_NEW YUV411V_to_RGB565_cc_mb;

YUV422INTERLACETORGB16_MB_FUNC YUYVToRGB16_MB_rotation_no;
YUV422INTERLACETORGB16_MB_FUNC YVYUToRGB16_MB_rotation_no;
YUV422INTERLACETORGB16_MB_FUNC UYVYToRGB16_MB_rotation_no;
YUV422INTERLACETORGB16_MB_FUNC VYUYToRGB16_MB_rotation_no;
YUV422INTERLACETORGB16_MB_FUNC YUYV2ToRGB16_MB_rotation_no;
YUV422INTERLACETORGB16_MB_FUNC YVYU2ToRGB16_MB_rotation_no;
YUV422INTERLACETORGB16_MB_FUNC UYVY2ToRGB16_MB_rotation_no;
YUV422INTERLACETORGB16_MB_FUNC VYUY2ToRGB16_MB_rotation_no;

//Resize
void cc_yuv420_mb_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
						   VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride);

void cc_yuv420_mb_s_c_3quarter(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v,VO_U8 *dst, VO_S32 src_stride, 
							   VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);

void cc_yuv420_mb_s_c_1point5(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							  VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);

void cc_yuv420_mb_s_c_double(VO_S32 width, VO_S32 height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride);

void cc_yuv420_rgb32_mb_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
								 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_argb32_mb_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
								 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);

//Rotation
void cc_yuv420_mb_s_l90_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							   VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride);

void cc_yuv420_mb_s_l90_c_3quarter(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v,VO_U8 *dst, VO_S32 src_stride, 
								   VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);

void cc_yuv420_mb_s_l90_c_1point5(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
								  VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);

void cc_yuv420_mb_s_l90_c_double(VO_S32 width, VO_S32 height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
								 VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride);

void cc_yuv420_mb_s_r90_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							   VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride);

void cc_yuv420_mb_s_r90_c_3quarter(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v,VO_U8 *dst, VO_S32 src_stride, 
								   VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);

void cc_yuv420_mb_s_r90_c_1point5(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
								  VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);

void cc_yuv420_mb_s_r90_c_double(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v,VO_U8 *dst, VO_S32 src_stride, 
								 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride);

void cc_yuv420_rgb32_mb_rotation_90l_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
											 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_rgb32_mb_rotation_90r_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
											  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride);

void cc_yuv420_argb32_mb_rotation_90l_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
											  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_argb32_mb_rotation_90r_s_c_half(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
											  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par, const VO_S32 uin_stride, const VO_S32 vin_stride);

VOCCRETURNCODE NewYUVPlanarToRGB16_normal(ClrConvData *conv_data, const VO_U32 isRGB565, const VO_U32 is420Planar,struct CC_HND * cc_hnd);
VOCCRETURNCODE NewYUVPlanarToRGB24_normal(ClrConvData *conv_data, const VO_U32 isRGB565, const VO_U32 is420Planar,struct CC_HND * cc_hnd);
VOCCRETURNCODE NewYUVPlanarToRGB32_normal(ClrConvData *conv_data, const VO_U32 isRGB565, const VO_U32 is420Planar,struct CC_HND * cc_hnd);
VOCCRETURNCODE NewYUVPlanarToARGB32_normal(ClrConvData *conv_data, const VO_U32 isRGB565, const VO_U32 is420Planar,struct CC_HND * cc_hnd);

YUV422INTERLACETORGB16_MB_FUNC YUYVToRGB16_MB_rotation_90L;
YUV422INTERLACETORGB16_MB_FUNC YUYVToRGB16_MB_rotation_90R;
YUV422INTERLACETORGB16_MB_FUNC YUYVToRGB16_MB_rotation_180;

YUV422INTERLACETORGB16_MB_FUNC YVYUToRGB16_MB_rotation_90L;
YUV422INTERLACETORGB16_MB_FUNC YVYUToRGB16_MB_rotation_90R;
YUV422INTERLACETORGB16_MB_FUNC YVYUToRGB16_MB_rotation_180;

YUV422INTERLACETORGB16_MB_FUNC UYVYToRGB16_MB_rotation_90L;
YUV422INTERLACETORGB16_MB_FUNC UYVYToRGB16_MB_rotation_90R;
YUV422INTERLACETORGB16_MB_FUNC UYVYToRGB16_MB_rotation_180;

YUV422INTERLACETORGB16_MB_FUNC VYUYToRGB16_MB_rotation_90L;
YUV422INTERLACETORGB16_MB_FUNC VYUYToRGB16_MB_rotation_90R;
YUV422INTERLACETORGB16_MB_FUNC VYUYToRGB16_MB_rotation_180;

YUV422INTERLACETORGB16_MB_FUNC YUYV2ToRGB16_MB_rotation_90L;
YUV422INTERLACETORGB16_MB_FUNC YUYV2ToRGB16_MB_rotation_90R;
YUV422INTERLACETORGB16_MB_FUNC YUYV2ToRGB16_MB_rotation_180;

YUV422INTERLACETORGB16_MB_FUNC YVYU2ToRGB16_MB_rotation_90L;
YUV422INTERLACETORGB16_MB_FUNC YVYU2ToRGB16_MB_rotation_90R;
YUV422INTERLACETORGB16_MB_FUNC YVYU2ToRGB16_MB_rotation_180;

YUV422INTERLACETORGB16_MB_FUNC UYVY2ToRGB16_MB_rotation_90L;
YUV422INTERLACETORGB16_MB_FUNC UYVY2ToRGB16_MB_rotation_90R;
YUV422INTERLACETORGB16_MB_FUNC UYVY2ToRGB16_MB_rotation_180;


YUV422INTERLACETORGB16_MB_FUNC VYUY2ToRGB16_MB_rotation_90L;
YUV422INTERLACETORGB16_MB_FUNC VYUY2ToRGB16_MB_rotation_90R;
YUV422INTERLACETORGB16_MB_FUNC VYUY2ToRGB16_MB_rotation_180;

extern YUV422INTERLACETORGB16_MB_FUNC_PTR YUV422INTERLACETORGB16_MB;
extern YUV422INTERLACETORGB16_MB_FUNC_PTR Yuv422Interlace_mb[8][4];
extern CC_MB_PTR YUV420PLANARTORGB16_NEWMB;
extern CC_MB_PTR YUV420PLANARTORGB24_NEWMB;
extern CC_MB_PTR YUV420PLANARTORGB32_NEWMB;
extern CC_MB_PTR YUV420PLANARTOARGB32_NEWMB;

#if defined(VOARMV4)
CC_MB cc_rgb32_mb_all_arm; 
CC_MB cc_argb32_mb_all_arm;
CC_MB cc_mb_16x16_arm;
CC_MB yuv420_rgb24_mb_16x16_c;
CC_MB_NEW YUV444_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW YUV422_21_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW YUV411_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW YUV411V_to_RGB565_cc_mb_16x16_c;


void cc_yuv420_mb_s_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
						VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv444_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv422_21_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
								VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv411_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv411V_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_to_rgb32_mb_s_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							     VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_to_argb32_mb_s_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							      VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);


void cc_yuv420_mb_s_l90_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_mb_s_r90_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_mb_s_180_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);


void cc_yuv420_to_rgb32_mb_s_l90_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										 const VO_S32 vin_stride);
void cc_yuv420_to_rgb32_mb_s_r90_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										 const VO_S32 vin_stride);
void cc_yuv420_to_rgb32_mb_s_180_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										 const VO_S32 vin_stride);

void cc_yuv420_to_argb32_mb_s_l90_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										  const VO_S32 vin_stride);
void cc_yuv420_to_argb32_mb_s_r90_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										  const VO_S32 vin_stride);
void cc_yuv420_to_argb32_mb_s_180_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										  const VO_S32 vin_stride);

CC_MB cc_mb_16x16_l90_arm;
CC_MB cc_mb_16x16_r90_arm;
CC_MB cc_mb_16x16_180_arm;

CC_MB cc_rgb24_mb_16x16_180_c;
CC_MB cc_rgb24_mb_16x16_l90_c;
CC_MB cc_rgb24_mb_16x16_r90_c;

CC_MB cc_rgb32_mb_16x16_180_arm;
CC_MB cc_rgb32_mb_16x16_l90_arm;
CC_MB cc_rgb32_mb_16x16_r90_arm;

CC_MB cc_argb32_mb_16x16_180_arm;
CC_MB cc_argb32_mb_16x16_l90_arm;
CC_MB cc_argb32_mb_16x16_r90_arm;

#define cc_mb_16x16			               cc_mb_16x16_arm
#define yuv420_rgb24_mb_16x16              yuv420_rgb24_mb_16x16_c
#define YUV444_to_RGB565_cc_mb_16x16       YUV444_to_RGB565_cc_mb_16x16_c
#define YUV422_21_to_RGB565_cc_mb_16x16    YUV422_21_to_RGB565_cc_mb_16x16_c
#define YUV411_to_RGB565_cc_mb_16x16       YUV411_to_RGB565_cc_mb_16x16_c
#define YUV411V_to_RGB565_cc_mb_16x16      YUV411V_to_RGB565_cc_mb_16x16_c
#define cc_rgb32_mb_16x16			       cc_rgb32_mb_all_arm
#define cc_argb32_mb_16x16			       cc_argb32_mb_all_arm

#define cc_yuv420_mb_s		            cc_yuv420_mb_s_arm
#define yuv444_to_rgb565_mb_s           yuv444_to_rgb565_mb_s_c
#define yuv422_21_to_rgb565_mb_s        yuv422_21_to_rgb565_mb_s_c
#define yuv411_to_rgb565_mb_s           yuv411_to_rgb565_mb_s_c
#define yuv411V_to_rgb565_mb_s          yuv411V_to_rgb565_mb_s_c
#define cc_yuv420_rgb32_mb_s		    cc_yuv420_to_rgb32_mb_s_arm
#define cc_yuv420_argb32_mb_s		    cc_yuv420_to_argb32_mb_s_arm


#define YUV420PlanarToRGB16_NEWMB_rotation_90L cc_mb_16x16_l90_arm
#define YUV420PlanarToRGB16_NEWMB_rotation_90R cc_mb_16x16_r90_arm
#define YUV420PlanarToRGB16_NEWMB_rotation_180 cc_mb_16x16_180_arm

#define YUV420PlanarToRGB24_NEWMB_rotation_90L cc_rgb24_mb_16x16_l90_c
#define YUV420PlanarToRGB24_NEWMB_rotation_90R cc_rgb24_mb_16x16_r90_c
#define YUV420PlanarToRGB24_NEWMB_rotation_180 cc_rgb24_mb_16x16_180_c

#define YUV420PlanarToRGB32_NEWMB_rotation_90L cc_rgb32_mb_16x16_l90_arm
#define YUV420PlanarToRGB32_NEWMB_rotation_90R cc_rgb32_mb_16x16_r90_arm
#define YUV420PlanarToRGB32_NEWMB_rotation_180 cc_rgb32_mb_16x16_180_arm

#define YUV420PlanarToARGB32_NEWMB_rotation_90L cc_argb32_mb_16x16_l90_arm
#define YUV420PlanarToARGB32_NEWMB_rotation_90R cc_argb32_mb_16x16_r90_arm
#define YUV420PlanarToARGB32_NEWMB_rotation_180 cc_argb32_mb_16x16_180_arm

#define cc_yuv420_mb_rotation_90l_s		cc_yuv420_mb_s_l90_arm
#define cc_yuv420_mb_rotation_90r_s		cc_yuv420_mb_s_r90_arm
#define cc_yuv420_mb_rotation_180_s		cc_yuv420_mb_s_180_arm

#define cc_yuv420_rgb32_mb_rotation_90l_s		cc_yuv420_to_rgb32_mb_s_l90_arm
#define cc_yuv420_rgb32_mb_rotation_90r_s		cc_yuv420_to_rgb32_mb_s_r90_arm
#define cc_yuv420_rgb32_mb_rotation_180_s		cc_yuv420_to_rgb32_mb_s_180_arm

#define cc_yuv420_argb32_mb_rotation_90l_s		cc_yuv420_to_argb32_mb_s_l90_arm
#define cc_yuv420_argb32_mb_rotation_90r_s		cc_yuv420_to_argb32_mb_s_r90_arm
#define cc_yuv420_argb32_mb_rotation_180_s		cc_yuv420_to_argb32_mb_s_180_arm
#elif defined(VOARMV6)
CC_MB cc_rgb32_mb_all_arm; 
CC_MB cc_argb32_mb_all_arm;
CC_MB voyuv420torgb16_arm;
CC_MB voyuv420torgb24_arm;
CC_MB_NEW YUV444_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW YUV422_21_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW YUV411_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW YUV411V_to_RGB565_cc_mb_16x16_c;

void cc_yuv420_mb_s_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
						VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv444_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv422_21_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
								VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv411_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv411V_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_rgb32_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_to_argb32_mb_s_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);


void cc_yuv420_mb_s_l90_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_mb_s_r90_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_mb_s_180_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);


void cc_yuv420_to_rgb32_mb_s_l90_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										 const VO_S32 vin_stride);
void cc_yuv420_to_rgb32_mb_s_r90_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										 const VO_S32 vin_stride);
void cc_yuv420_to_rgb32_mb_s_180_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										 const VO_S32 vin_stride);

void cc_yuv420_to_argb32_mb_s_l90_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										  const VO_S32 vin_stride);
void cc_yuv420_to_argb32_mb_s_r90_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										  const VO_S32 vin_stride);
void cc_yuv420_to_argb32_mb_s_180_arm(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										  const VO_S32 vin_stride);

CC_MB cc_mb_16x16_l90_arm;
CC_MB cc_mb_16x16_r90_arm;
CC_MB cc_mb_16x16_180_arm;

CC_MB cc_rgb24_mb_16x16_180_c;
CC_MB cc_rgb24_mb_16x16_l90_c;
CC_MB cc_rgb24_mb_16x16_r90_c;

CC_MB cc_rgb32_mb_16x16_180_arm;
CC_MB cc_rgb32_mb_16x16_l90_arm;
CC_MB cc_rgb32_mb_16x16_r90_arm;

CC_MB cc_argb32_mb_16x16_180_arm;
CC_MB cc_argb32_mb_16x16_l90_arm;
CC_MB cc_argb32_mb_16x16_r90_arm;

#define cc_mb_16x16			               voyuv420torgb16_arm
#define yuv420_rgb24_mb_16x16              voyuv420torgb24_arm
#define YUV444_to_RGB565_cc_mb_16x16       YUV444_to_RGB565_cc_mb_16x16_c
#define YUV422_21_to_RGB565_cc_mb_16x16    YUV422_21_to_RGB565_cc_mb_16x16_c
#define YUV411_to_RGB565_cc_mb_16x16       YUV411_to_RGB565_cc_mb_16x16_c
#define YUV411V_to_RGB565_cc_mb_16x16      YUV411V_to_RGB565_cc_mb_16x16_c
#define cc_rgb32_mb_16x16			       cc_rgb32_mb_all_arm
#define cc_argb32_mb_16x16			       cc_argb32_mb_all_arm

#define cc_yuv420_mb_s		               cc_yuv420_mb_s_arm
#define yuv444_to_rgb565_mb_s              yuv444_to_rgb565_mb_s_c
#define yuv422_21_to_rgb565_mb_s           yuv422_21_to_rgb565_mb_s_c
#define yuv411_to_rgb565_mb_s              yuv411_to_rgb565_mb_s_c
#define yuv411V_to_rgb565_mb_s             yuv411V_to_rgb565_mb_s_c
#define cc_yuv420_rgb32_mb_s		       cc_yuv420_rgb32_mb_s_c
#define cc_yuv420_argb32_mb_s		       cc_yuv420_to_argb32_mb_s_arm


#define YUV420PlanarToRGB16_NEWMB_rotation_90L cc_mb_16x16_l90_arm
#define YUV420PlanarToRGB16_NEWMB_rotation_90R cc_mb_16x16_r90_arm
#define YUV420PlanarToRGB16_NEWMB_rotation_180 cc_mb_16x16_180_arm

#define YUV420PlanarToRGB24_NEWMB_rotation_90L cc_rgb24_mb_16x16_l90_c
#define YUV420PlanarToRGB24_NEWMB_rotation_90R cc_rgb24_mb_16x16_r90_c
#define YUV420PlanarToRGB24_NEWMB_rotation_180 cc_rgb24_mb_16x16_180_c

#define YUV420PlanarToRGB32_NEWMB_rotation_90L cc_rgb32_mb_16x16_l90_arm
#define YUV420PlanarToRGB32_NEWMB_rotation_90R cc_rgb32_mb_16x16_r90_arm
#define YUV420PlanarToRGB32_NEWMB_rotation_180 cc_rgb32_mb_16x16_180_arm

#define YUV420PlanarToARGB32_NEWMB_rotation_90L cc_argb32_mb_16x16_l90_arm
#define YUV420PlanarToARGB32_NEWMB_rotation_90R cc_argb32_mb_16x16_r90_arm
#define YUV420PlanarToARGB32_NEWMB_rotation_180 cc_argb32_mb_16x16_180_arm

#define cc_yuv420_mb_rotation_90l_s		cc_yuv420_mb_s_l90_arm
#define cc_yuv420_mb_rotation_90r_s		cc_yuv420_mb_s_r90_arm
#define cc_yuv420_mb_rotation_180_s		cc_yuv420_mb_s_180_arm

#define cc_yuv420_rgb32_mb_rotation_90l_s		cc_yuv420_to_rgb32_mb_s_l90_arm
#define cc_yuv420_rgb32_mb_rotation_90r_s		cc_yuv420_to_rgb32_mb_s_r90_arm
#define cc_yuv420_rgb32_mb_rotation_180_s		cc_yuv420_to_rgb32_mb_s_180_arm

#define cc_yuv420_argb32_mb_rotation_90l_s		cc_yuv420_to_argb32_mb_s_l90_arm
#define cc_yuv420_argb32_mb_rotation_90r_s		cc_yuv420_to_argb32_mb_s_r90_arm
#define cc_yuv420_argb32_mb_rotation_180_s		cc_yuv420_to_argb32_mb_s_180_arm
#elif defined(VOARMV7)
extern YUV420PLANARTOYUV420_MB_ROTATION_PTR YUV420PLANARTOYUV420_MB_ROTATION;

YUV420PLANARTOYUV420_MB_ROTATION_PTR YUV420PLANARTOYUV420_MB_ROTATION;
YUV420PLANARTOYUV420_MB_ROTATION_FUNC cc_yuv2yuv_8x8_L90_armv7;
YUV420PLANARTOYUV420_MB_ROTATION_FUNC cc_yuv2yuv_8x8_R90_armv7;
YUV420PLANARTOYUV420_MB_ROTATION_FUNC cc_yuv2yuv_8x8_R180_armv7;

CC_MB voyuv420torgb32_8nx2n_armv7_new; 
CC_MB voyuv420toargb32_8nx2n_armv7_new;
CC_MB voyuv420torgb32_8nx2n_armv7; 
CC_MB voyuv420toargb32_8nx2n_armv7;

CC_MB cc_rgb24_mb_16x16_180_c;
CC_MB cc_rgb24_mb_16x16_l90_c;
CC_MB cc_rgb24_mb_16x16_r90_c;

CC_MB cc_rgb32_mb_16x16_180_c;
CC_MB cc_rgb32_mb_16x16_l90_c;
CC_MB cc_rgb32_mb_16x16_r90_c;

CC_MB cc_argb32_mb_16x16_180_c;
CC_MB cc_argb32_mb_16x16_l90_c;
CC_MB cc_argb32_mb_16x16_r90_c;

CC_MB cc_mb_16x16_180_arm;
CC_MB voyuv420torgb24_armv7;
CC_MB voyuv420torgb16_8nx2n_armv7;
CC_MB_NEW YUV444_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW YUV422_21_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW YUV411_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW YUV411_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW YUV411V_to_RGB565_cc_mb_16x16_c;

void cc_yuv420_mb_s_arm(VO_S32 width, VO_S32 height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
						VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv444_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv422_21_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
								VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv411_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv411V_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_rgb32_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_argb32_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);


void cc_yuv2yuv_8x2n_s_armv7(VO_S32 width, VO_S32 height, VO_U8 *y, VO_U8 *u, VO_U8 *v, VO_U8 *dst,VO_S32 in_stride,VO_S32 yuv_buffer_stride,
							 VO_S32 *x_scale_tab, VO_S32 *y_scale_tab, VO_S32 uin_stride,VO_S32 vin_stride,VO_U8 *dsty,VO_U8 *dstu, VO_U8 *dstv);
void cc_yuv2yuv_8x8_s_armv7(VO_S32 width, VO_S32 height, VO_U8 *y, VO_U8 *u, VO_U8 *v, VO_U8 *dst,VO_S32 in_stride,VO_S32 yuv_buffer_stride,
							 VO_S32 *x_scale_tab, VO_S32 *y_scale_tab, VO_S32 uin_stride,VO_S32 vin_stride,VO_U8 *dsty,VO_U8 *dstu, VO_U8 *dstv);
void cc_yuv2yuv_8x8_sL90_armv7(VO_S32 width, VO_S32 height, VO_U8 *y, VO_U8 *u, VO_U8 *v, VO_U8 *dst,VO_S32 in_stride,VO_S32 yuv_buffer_stride,
							   VO_S32 *x_scale_tab, VO_S32 *y_scale_tab, VO_S32 uin_stride,VO_S32 vin_stride,VO_U8 *dsty,VO_U8 *dstu, VO_U8 *dstv);
void cc_yuv2yuv_8x2n_sL90_armv7(VO_S32 width, VO_S32 height, VO_U8 *y, VO_U8 *u, VO_U8 *v, VO_U8 *dst,VO_S32 in_stride,VO_S32 yuv_buffer_stride,
								VO_S32 *x_scale_tab, VO_S32 *y_scale_tab, VO_S32 uin_stride,VO_S32 vin_stride,VO_U8 *dsty,VO_U8 *dstu, VO_U8 *dstv);
void cc_yuv2yuv_8x8_sR90_armv7(VO_S32 width, VO_S32 height, VO_U8 *y, VO_U8 *u, VO_U8 *v, VO_U8 *dst,VO_S32 in_stride,VO_S32 yuv_buffer_stride,
							   VO_S32 *x_scale_tab, VO_S32 *y_scale_tab, VO_S32 uin_stride,VO_S32 vin_stride,VO_U8 *dsty,VO_U8 *dstu, VO_U8 *dstv);
void cc_yuv2yuv_8x2n_sR90_armv7(VO_S32 width, VO_S32 height, VO_U8 *y, VO_U8 *u, VO_U8 *v, VO_U8 *dst,VO_S32 in_stride,VO_S32 yuv_buffer_stride,
								VO_S32 *x_scale_tab, VO_S32 *y_scale_tab, VO_S32 uin_stride,VO_S32 vin_stride,VO_U8 *dsty,VO_U8 *dstu, VO_U8 *dstv);
void cc_yuv2yuv_8x8_sR180_armv7(VO_S32 width, VO_S32 height, VO_U8 *y, VO_U8 *u, VO_U8 *v, VO_U8 *dst,VO_S32 in_stride,VO_S32 yuv_buffer_stride,
								VO_S32 *x_scale_tab, VO_S32 *y_scale_tab, VO_S32 uin_stride,VO_S32 vin_stride,VO_U8 *dsty,VO_U8 *dstu, VO_U8 *dstv);
void cc_yuv2yuv_8x2n_sR180_armv7(VO_S32 width, VO_S32 height, VO_U8 *y, VO_U8 *u, VO_U8 *v, VO_U8 *dst,VO_S32 in_stride,VO_S32 yuv_buffer_stride,
								 VO_S32 *x_scale_tab, VO_S32 *y_scale_tab, VO_S32 uin_stride,VO_S32 vin_stride,VO_U8 *dsty,VO_U8 *dstu, VO_U8 *dstv);


void cc_yuv420_rgb32_mb_rotation_90l_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										 const VO_S32 vin_stride);
void cc_yuv420_rgb32_mb_rotation_90r_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										 const VO_S32 vin_stride);
void cc_yuv420_rgb32_mb_rotation_180_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										 const VO_S32 vin_stride);

void cc_yuv420_argb32_mb_rotation_90l_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										  const VO_S32 vin_stride);
void cc_yuv420_argb32_mb_rotation_90r_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										  const VO_S32 vin_stride);
void cc_yuv420_argb32_mb_rotation_180_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										  const VO_S32 vin_stride);


void cc_yuv2yuv_16x16_s_x86(VO_S32 width, VO_S32 height, VO_U8 *y, VO_U8 *u, VO_U8 *v, VO_U8 *dst,VO_S32 in_stride,VO_S32 yuv_buffer_stride,
							 VO_S32 *x_scale_tab, VO_S32 *y_scale_tab, VO_S32 uin_stride,VO_S32 vin_stride,VO_U8 *dsty,VO_U8 *dstu, VO_U8 *dstv);


void voyuv420toargb32_16nx2n_x86( VO_U8 *dsty, VO_U8 *dstu, VO_U8 *dstv, VO_S32 in_stride, VO_S32 uin_stride, 
	                                     int width, int height, VO_U32 *rgb, VO_S32 srgb );

void voyuv420targb32_16nx2n_x86( VO_U8 *dsty, VO_U8 *dstu, VO_U8 *dstv, VO_S32 in_stride, VO_S32 uin_stride, 
	                                     int width, int height, VO_U32 *rgb, VO_S32 srgb );


#define cc_mb_16x16			               voyuv420torgb16_8nx2n_armv7
#define yuv420_rgb24_mb_16x16              voyuv420torgb24_armv7
#define YUV444_to_RGB565_cc_mb_16x16       YUV444_to_RGB565_cc_mb_16x16_c
#define YUV422_21_to_RGB565_cc_mb_16x16    YUV422_21_to_RGB565_cc_mb_16x16_c
#define YUV411_to_RGB565_cc_mb_16x16       YUV411_to_RGB565_cc_mb_16x16_c
#define YUV411V_to_RGB565_cc_mb_16x16      YUV411V_to_RGB565_cc_mb_16x16_c
#define cc_rgb32_mb_16x16			       voyuv420torgb32_8nx2n_armv7
#define cc_argb32_mb_16x16			       voyuv420toargb32_8nx2n_armv7
#define cc_rgb32_neon			       voyuv420torgb32_8nx2n_armv7_new
#define cc_argb32_neon			       voyuv420toargb32_8nx2n_armv7_new


#define yuv444_to_rgb565_mb_s           yuv444_to_rgb565_mb_s_c
#define yuv422_21_to_rgb565_mb_s        yuv422_21_to_rgb565_mb_s_c
#define yuv411_to_rgb565_mb_s           yuv411_to_rgb565_mb_s_c
#define yuv411V_to_rgb565_mb_s          yuv411V_to_rgb565_mb_s_c
#define cc_yuv420_rgb32_mb_s		    cc_yuv420_rgb32_mb_s_c
#define cc_yuv420_argb32_mb_s		    cc_yuv420_argb32_mb_s_c

#define YUV420PlanarToRGB24_NEWMB_rotation_90L cc_rgb24_mb_16x16_l90_c
#define YUV420PlanarToRGB24_NEWMB_rotation_90R cc_rgb24_mb_16x16_r90_c
#define YUV420PlanarToRGB24_NEWMB_rotation_180 cc_rgb24_mb_16x16_180_c

#define YUV420PlanarToRGB32_NEWMB_rotation_90L cc_rgb32_mb_16x16_l90_c
#define YUV420PlanarToRGB32_NEWMB_rotation_90R cc_rgb32_mb_16x16_r90_c
#define YUV420PlanarToRGB32_NEWMB_rotation_180 cc_rgb32_mb_16x16_180_c

#define YUV420PlanarToARGB32_NEWMB_rotation_90L cc_argb32_mb_16x16_l90_c
#define YUV420PlanarToARGB32_NEWMB_rotation_90R cc_argb32_mb_16x16_r90_c
#define YUV420PlanarToARGB32_NEWMB_rotation_180 cc_argb32_mb_16x16_180_c

#define cc_yuv420_mb_rotation_90l_s		cc_yuv420_mb_rotation_90l_s_c
#define cc_yuv420_mb_rotation_90r_s		cc_yuv420_mb_rotation_90r_s_c
#define cc_yuv420_mb_rotation_180_s		cc_yuv420_mb_rotation_180_s_c

#define cc_yuv420_rgb32_mb_rotation_90l_s		cc_yuv420_rgb32_mb_rotation_90l_s_c
#define cc_yuv420_rgb32_mb_rotation_90r_s		cc_yuv420_rgb32_mb_rotation_90r_s_c
#define cc_yuv420_rgb32_mb_rotation_180_s		cc_yuv420_rgb32_mb_rotation_180_s_c

#define cc_yuv420_argb32_mb_rotation_90l_s		cc_yuv420_argb32_mb_rotation_90l_s_c
#define cc_yuv420_argb32_mb_rotation_90r_s		cc_yuv420_argb32_mb_rotation_90r_s_c
#define cc_yuv420_argb32_mb_rotation_180_s		cc_yuv420_argb32_mb_rotation_180_s_c
#else
CC_MB cc_mb_16x16_c;
CC_MB cc_rgb32_mb_16x16_c;
CC_MB cc_argb32_mb_16x16_c;
CC_MB yuv420_rgb24_mb_16x16_c;
CC_MB_NEW YUV444_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW YUV422_21_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW YUV411_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW YUV411V_to_RGB565_cc_mb_16x16_c;

void cc_yuv420_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
					  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv444_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv422_21_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
								VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv411_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void yuv411V_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_rgb32_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void cc_yuv420_argb32_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride,
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);


void cc_yuv420_mb_rotation_90l_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
								   VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
								   const VO_S32 vin_stride);
void cc_yuv420_mb_rotation_90r_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
								   VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
								   const VO_S32 vin_stride);
void cc_yuv420_mb_rotation_180_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
								   VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
								   const VO_S32 vin_stride);

void cc_yuv420_rgb32_mb_rotation_90l_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										 const VO_S32 vin_stride);
void cc_yuv420_rgb32_mb_rotation_90r_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										 const VO_S32 vin_stride);
void cc_yuv420_rgb32_mb_rotation_180_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										 const VO_S32 vin_stride);

void cc_yuv420_argb32_mb_rotation_90l_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										  const VO_S32 vin_stride);
void cc_yuv420_argb32_mb_rotation_90r_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										  const VO_S32 vin_stride);
void cc_yuv420_argb32_mb_rotation_180_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
										  VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, 
										  const VO_S32 vin_stride);

CC_MB cc_mb_16x16_180_c;
CC_MB cc_mb_16x16_l90_c;
CC_MB cc_mb_16x16_r90_c;

CC_MB cc_rgb24_mb_16x16_180_c;
CC_MB cc_rgb24_mb_16x16_l90_c;
CC_MB cc_rgb24_mb_16x16_r90_c;

CC_MB cc_rgb32_mb_16x16_180_c;
CC_MB cc_rgb32_mb_16x16_l90_c;
CC_MB cc_rgb32_mb_16x16_r90_c;

CC_MB cc_argb32_mb_16x16_180_c;
CC_MB cc_argb32_mb_16x16_l90_c;
CC_MB cc_argb32_mb_16x16_r90_c;

#define cc_mb_16x16			            cc_mb_16x16_c
#define cc_rgb32_mb_16x16			    cc_rgb32_mb_16x16_c
#define cc_argb32_mb_16x16			    cc_argb32_mb_16x16_c
#define yuv420_rgb24_mb_16x16           yuv420_rgb24_mb_16x16_c
#define YUV444_to_RGB565_cc_mb_16x16    YUV444_to_RGB565_cc_mb_16x16_c
#define YUV422_21_to_RGB565_cc_mb_16x16 YUV422_21_to_RGB565_cc_mb_16x16_c
#define YUV411_to_RGB565_cc_mb_16x16    YUV411_to_RGB565_cc_mb_16x16_c
#define YUV411V_to_RGB565_cc_mb_16x16   YUV411V_to_RGB565_cc_mb_16x16_c

#define YUV420PlanarToRGB16_NEWMB_rotation_90L cc_mb_16x16_l90_c
#define YUV420PlanarToRGB16_NEWMB_rotation_90R cc_mb_16x16_r90_c
#define YUV420PlanarToRGB16_NEWMB_rotation_180 cc_mb_16x16_180_c

#define YUV420PlanarToRGB24_NEWMB_rotation_90L cc_rgb24_mb_16x16_l90_c
#define YUV420PlanarToRGB24_NEWMB_rotation_90R cc_rgb24_mb_16x16_r90_c
#define YUV420PlanarToRGB24_NEWMB_rotation_180 cc_rgb24_mb_16x16_180_c

#define YUV420PlanarToRGB32_NEWMB_rotation_90L cc_rgb32_mb_16x16_l90_c
#define YUV420PlanarToRGB32_NEWMB_rotation_90R cc_rgb32_mb_16x16_r90_c
#define YUV420PlanarToRGB32_NEWMB_rotation_180 cc_rgb32_mb_16x16_180_c

#define YUV420PlanarToARGB32_NEWMB_rotation_90L cc_argb32_mb_16x16_l90_c
#define YUV420PlanarToARGB32_NEWMB_rotation_90R cc_argb32_mb_16x16_r90_c
#define YUV420PlanarToARGB32_NEWMB_rotation_180 cc_argb32_mb_16x16_180_c

#define cc_yuv420_mb_s		            cc_yuv420_mb_s_c
#define yuv444_to_rgb565_mb_s           yuv444_to_rgb565_mb_s_c
#define yuv422_21_to_rgb565_mb_s        yuv422_21_to_rgb565_mb_s_c
#define yuv411_to_rgb565_mb_s           yuv411_to_rgb565_mb_s_c
#define yuv411V_to_rgb565_mb_s          yuv411V_to_rgb565_mb_s_c
#define cc_yuv420_rgb32_mb_s		    cc_yuv420_rgb32_mb_s_c
#define cc_yuv420_argb32_mb_s		    cc_yuv420_argb32_mb_s_c

#define cc_yuv420_mb_rotation_90l_s		cc_yuv420_mb_rotation_90l_s_c
#define cc_yuv420_mb_rotation_90r_s		cc_yuv420_mb_rotation_90r_s_c
#define cc_yuv420_mb_rotation_180_s		cc_yuv420_mb_rotation_180_s_c

#define cc_yuv420_rgb32_mb_rotation_90l_s		cc_yuv420_rgb32_mb_rotation_90l_s_c
#define cc_yuv420_rgb32_mb_rotation_90r_s		cc_yuv420_rgb32_mb_rotation_90r_s_c
#define cc_yuv420_rgb32_mb_rotation_180_s		cc_yuv420_rgb32_mb_rotation_180_s_c

#define cc_yuv420_argb32_mb_rotation_90l_s		cc_yuv420_argb32_mb_rotation_90l_s_c
#define cc_yuv420_argb32_mb_rotation_90r_s		cc_yuv420_argb32_mb_rotation_90r_s_c
#define cc_yuv420_argb32_mb_rotation_180_s		cc_yuv420_argb32_mb_rotation_180_s_c
#endif
             
#endif							/* _YUV_TO_RGB_MB_H_ */
