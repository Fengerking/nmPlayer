/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __YUV2RGB16_MB_H
#define __YUV2RGB16_MB_H

#if defined(VOARMV4) || defined(VOARMV6) || defined(VOARMV7)
#define cc_yuv420_mb_s		cc_yuv420_mb_s_arm
#define cc_yuv420_mb_s_180		cc_yuv420_mb_s_180_arm
#define cc_yuv420_mb_s_l90	cc_yuv420_mb_s_l90_arm
#define cc_yuv420_mb_s_r90	cc_yuv420_mb_s_r90_arm
#define Yuv2yuv_R90_inter Yuv2yuv_R90_inter_arm
#define Yuv2yuv_L90_inter Yuv2yuv_L90_inter_arm
#define Yuv2yuv_180_inter Yuv2yuv_180_inter_arm
#elif defined(WMMX)
#define cc_yuv420_mb_s		cc_yuv420_mb_s_wmmx
#define cc_yuv420_mb_s_l90	cc_yuv420_mb_s_l90_wmmx
#define cc_yuv420_mb_s_r90	cc_yuv420_mb_s_r90_wmmx
#else
#define cc_yuv420_mb_s		cc_yuv420_mb_s_c
#define cc_yuv420_mb_s_180		cc_yuv420_mb_s_180_c
#define cc_yuv420_mb_s_l90	cc_yuv420_mb_s_l90_c
#define cc_yuv420_mb_s_r90	cc_yuv420_mb_s_r90_c
#define Yuv2yuv_R90_inter Yuv2yuv_R90_inter_c
#define Yuv2yuv_L90_inter Yuv2yuv_L90_inter_c
#define Yuv2yuv_180_inter Yuv2yuv_180_inter_c
#endif

/*
#define cc_yuv420_mb_s		cc_yuv420_mb_s_c
#define cc_yuv420_mb_s_l90	cc_yuv420_mb_s_l90_c
#define cc_yuv420_mb_s_r90	cc_yuv420_mb_s_r90_arm//cc_yuv420_mb_s_r90_c
*/
//void init_scale(float x_scale, int out_width, int *scale_tab);
//void cc_mb_s(int width, int height, VO_U8 *src_y, VO_U8 *src_u, VO_U8 *src_v, 
//		   VO_U8 *dst, const VO_S32 src_stride, const VO_S32 dst_stride,
//		   VO_S32 *x_scale_par, VO_S32 *y_scale_par);
#endif//__YUV2RGB16_MB_H
