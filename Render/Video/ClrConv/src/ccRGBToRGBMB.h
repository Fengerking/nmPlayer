/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _RGB_TO_RGB_MB_H_
#define _RGB_TO_RGB_MB_H_

#include "voYYDef_CC.h"
#include "voColorConversion.h"
#include "ccInternal.h"

typedef void(RGB888PLANARTORGB16_MB_FUNC)(VO_U8 *psrc_y, VO_U8 *psrc_u, VO_U8 *psrc_v, const VO_S32 in_stride,
										  VO_U8 *out_buf, const VO_S32 out_stride, const VO_S32* const param_tab,
										  const VO_U32 is420planar, const VO_U32 isRGB565);
typedef RGB888PLANARTORGB16_MB_FUNC *RGB888PLANARTORGB16_MB_FUNC_PTR;

typedef void(CC_RGB565_MB)(VO_S32 width, VO_S32 height, VO_U8 *src, VO_U8 *dst,VO_S32 src_stride, VO_S32 dst_stride,
						   VO_S32 *x_scale_par,VO_S32 *y_scale_par);
typedef CC_RGB565_MB *CC_RGB565_MB_PTR;

CC_MB_NEW RGB888Planar_to_RGB565_cc_mb;

//Resize
void cc_rgb24_mb_s(VO_S32 width, VO_S32 height, VO_U8 *src, VO_U8 *dst, VO_S32 src_stride, VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par);

CC_RGB565_MB cc_rgb565_mb_s;

//Rotation
RGB888PLANARTORGB16_MB_FUNC RGB888PlanarToRGB16_MB_rotation_90L;
RGB888PLANARTORGB16_MB_FUNC RGB888PlanarToRGB16_MB_rotation_90R;
RGB888PLANARTORGB16_MB_FUNC RGB888PlanarToRGB16_MB_rotation_180;

void cc_rgb24_mb_s_l90(VO_S32 width, VO_S32 height, VO_U8 *src, VO_U8 *dst,VO_S32 src_stride, VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par);
void cc_rgb24_mb_s_r90(VO_S32 width, VO_S32 height, VO_U8 *src, VO_U8 *dst,VO_S32 src_stride, VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par);

CC_RGB565_MB cc_rgb565_mb_s_l90;
CC_RGB565_MB cc_rgb565_mb_s_r90;

/*[isRisize][isRT90]*/
extern CLRCONV_FUNC_PTR RGB16_RESIZE[3][2];
extern CC_RGB565_MB_PTR RGB565TORGB565_MB;
RGB888PLANARTORGB16_MB_FUNC_PTR RGB888PLANARTORGB16_MB;

#ifdef COLCONVENABLE

void rgb888_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void rgb888_to_rgb565_mb_rotation_90l_s_c(int width, int height, VO_U8 *src_r, VO_U8 *src_g, VO_U8 *src_b, VO_U8 *dst, 
										  VO_S32 red_stride,VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par, 
										  const VO_S32 green_stride, const VO_S32 blue_stride);
void rgb888_to_rgb565_mb_rotation_90r_s_c(int width, int height, VO_U8 *src_r, VO_U8 *src_g, VO_U8 *src_b, VO_U8 *dst, 
										  VO_S32 red_stride,VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par, 
										  const VO_S32 green_stride, const VO_S32 blue_stride);
void rgb888_to_rgb565_mb_rotation_180_s_c(int width, int height, VO_U8 *src_r, VO_U8 *src_g, VO_U8 *src_b, VO_U8 *dst, 
										  VO_S32 red_stride,VO_S32 dst_stride,VO_S32 *x_scale_par, VO_S32 *y_scale_par, 
										  const VO_S32 green_stride, const VO_S32 blue_stride);

#endif

#if defined(VOARMV4)
CC_MB_NEW RGB888Planar_to_RGB565_cc_mb_16x16_c;

void rgb888_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);


#define RGB888Planar_to_RGB565_cc_mb_16x16 RGB888Planar_to_RGB565_cc_mb_16x16_c
#define rgb888_to_rgb565_mb_s              rgb888_to_rgb565_mb_s_c
#define rgb888_to_rgb565_mb_rotation_90l_s rgb888_to_rgb565_mb_rotation_90l_s_c
#define rgb888_to_rgb565_mb_rotation_90r_s rgb888_to_rgb565_mb_rotation_90r_s_c
#define rgb888_to_rgb565_mb_rotation_180_s rgb888_to_rgb565_mb_rotation_180_s_c
#elif defined(VOARMV6)
CC_MB_NEW RGB888Planar_to_RGB565_cc_mb_16x16_c;
CC_MB_NEW rgb888_to_rgb16_8nx2n_armv7;


void rgb888_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);

#define RGB888Planar_to_RGB565_cc_mb_16x16 RGB888Planar_to_RGB565_cc_mb_16x16_c
#define rgb888_to_rgb565_mb_s              rgb888_to_rgb565_mb_s_c
#define rgb888_to_rgb565_mb_rotation_90l_s rgb888_to_rgb565_mb_rotation_90l_s_c
#define rgb888_to_rgb565_mb_rotation_90r_s rgb888_to_rgb565_mb_rotation_90r_s_c
#define rgb888_to_rgb565_mb_rotation_180_s rgb888_to_rgb565_mb_rotation_180_s_c
#elif defined(VOARMV7)
void rgb888_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void rgb888_to_s_rgb16_8x8_armv7(VO_S32 width, VO_S32 height, VO_U8 *y, VO_U8 *u, VO_U8 *v, VO_U8 *dst, VO_S32 in_stride,
								 VO_S32 out_stride, VO_S32 *x_scale_tab, VO_S32 *y_scale_tab, VO_S32 uin_stride, VO_S32 vin_stride);
void rgb888_to_s_rgb16_8x2n_armv7(VO_S32 width, VO_S32 height, VO_U8 *y, VO_U8 *u, VO_U8 *v, VO_U8 *dst, VO_S32 in_stride,
								  VO_S32 out_stride, VO_S32 *x_scale_tab, VO_S32 *y_scale_tab, VO_S32 uin_stride, VO_S32 vin_stride);

CC_MB_NEW rgb888_to_rgb16_8nx2n_armv7;

#define RGB888Planar_to_RGB565_cc_mb_16x16 rgb888_to_rgb16_8nx2n_armv7
#define rgb888_to_rgb565_mb_s              rgb888_to_rgb565_mb_s_c
#define rgb888_to_rgb565_mb_rotation_90l_s rgb888_to_rgb565_mb_rotation_90l_s_c
#define rgb888_to_rgb565_mb_rotation_90r_s rgb888_to_rgb565_mb_rotation_90r_s_c
#define rgb888_to_rgb565_mb_rotation_180_s rgb888_to_rgb565_mb_rotation_180_s_c
#else
CC_MB_NEW RGB888Planar_to_RGB565_cc_mb_16x16_c;

void rgb888_to_rgb565_mb_s_c(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, VO_U8 *dst, VO_S32 src_stride, 
							 VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,const VO_S32 uin_stride, const VO_S32 vin_stride);
void rgb888_to_rgb565_mb_rotation_90l_s_c(int width, int height, VO_U8 *src_r, VO_U8 *src_g, VO_U8 *src_b, VO_U8 *dst, 
										  VO_S32 red_stride, VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,
										  const VO_S32 green_stride, const VO_S32 blue_stride);
void rgb888_to_rgb565_mb_rotation_90r_s_c(int width, int height, VO_U8 *src_r, VO_U8 *src_g, VO_U8 *src_b, VO_U8 *dst, 
										  VO_S32 red_stride, VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,
										  const VO_S32 green_stride, const VO_S32 blue_stride);
void rgb888_to_rgb565_mb_rotation_180_s_c(int width, int height, VO_U8 *src_r, VO_U8 *src_g, VO_U8 *src_b, VO_U8 *dst, 
										  VO_S32 red_stride, VO_S32 dst_stride, VO_S32 *x_scale_par, VO_S32 *y_scale_par,
										  const VO_S32 green_stride, const VO_S32 blue_stride);

#define RGB888Planar_to_RGB565_cc_mb_16x16 RGB888Planar_to_RGB565_cc_mb_16x16_c
#define rgb888_to_rgb565_mb_s              rgb888_to_rgb565_mb_s_c
#define rgb888_to_rgb565_mb_rotation_90l_s rgb888_to_rgb565_mb_rotation_90l_s_c
#define rgb888_to_rgb565_mb_rotation_90r_s rgb888_to_rgb565_mb_rotation_90r_s_c
#define rgb888_to_rgb565_mb_rotation_180_s rgb888_to_rgb565_mb_rotation_180_s_c
#endif

#endif							/* _RGB_TO_RGB_MB_H_ */
