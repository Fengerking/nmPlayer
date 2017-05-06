/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _YUV_TO_YUV_H_
#define _YUV_TO_YUV_H_

#include "voYYDef_CC.h"
#include "voColorConversion.h"
#include "ccInternal.h"

//Rotation
void Yuv2yuv_R90_inter_c(VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride);
void Yuv2yuv_L90_inter_c(VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride);
void Yuv2yuv_180_inter_c(VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride);

extern void YUVPlanarRt90R(unsigned char *src, unsigned char *dst, const int width, const int height, const int in_stride, const int out_stride);
extern void YUVPlanarRt90L(unsigned char *src, unsigned char *dst, const int width, const int height, const int in_stride, const int out_stride);

/*YUV420Planar*/
VOCCRETURNCODE YUV420Planar_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420Planar_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420Planar_resize_anti_aliasing(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

/*YUV420Planar to YUYV*/
VOCCRETURNCODE YUVPlanarToYUYV_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToUYVY_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

/*YUV420Planar to YUV420Packed*/
VOCCRETURNCODE YUV420PlanarToYUV420Pack_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420PlanarToYUV420Pack_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

/*YUV420Planar to YUV420Packed2*/
VOCCRETURNCODE YUV420PlanarToYUV420Pack2_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420PlanarToYUV420Pack2_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

/*YUV420Packed*/
VOCCRETURNCODE YUV420Pack_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420Packed_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPacked_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420Packed_resize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

void YUVPlanarRt90L(unsigned char *src, unsigned char *dst, const int width, const int height, const int in_stride, const int out_stride);
void YUVPlanarRt90R(unsigned char *src, unsigned char *dst, const int width, const int height, const int in_stride, const int out_stride);

/*YUV422Packed*/
VOCCRETURNCODE YUV422Pack_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV422Packed_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV422Pack_noresize_Rt90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV422Packed_resize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

VOCCRETURNCODE YUV420PlanarToYUV420Pack_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420PlanarToYUV420Pack_RT180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420PlanarToYUV420Pack_resize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420PLANARToYUV420PACK_resize_RT180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

VOCCRETURNCODE YUV420PlanarToYUV420Pack2_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420PlanarToYUV420Pack2_RT180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420PlanarToYUV420Pack2_resize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420PlanarToYUV420Pack2_resize_RT180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

VOCCRETURNCODE YUV420Planar_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420Planar_RT180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420Planar_resize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV420Planar_resize_RT180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

extern CLRCONV_FUNC_PTR YUV422PACK_RESIZE[2][2];
extern CLRCONV_FUNC_PTR YUV420PLANAR_RESIZE[3][3];
extern CLRCONV_FUNC_PTR YUV420PACKED_RESIZE[2][2];
extern CLRCONV_FUNC_PTR YUVPlanarToYUYV[2][3];
extern CLRCONV_FUNC_PTR YUVPlanarToUYVY[2][3];
extern CLRCONV_FUNC_PTR YUV420PLANARTOYUV420PACK_RESIZE[3][3];
extern CLRCONV_FUNC_PTR YUV420PLANARTOYUV420PACK2_RESIZE[3][3];


#if defined(VOARMV4)
void Yuv2yuv_R90_inter_arm(VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride);
void Yuv2yuv_L90_inter_arm(VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride);
void Yuv2yuv_180_inter_arm(VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride);

#define Yuv2yuv_R90_inter            Yuv2yuv_R90_inter_arm
#define Yuv2yuv_L90_inter            Yuv2yuv_L90_inter_arm
#define Yuv2yuv_180_inter            Yuv2yuv_180_inter_arm
#elif defined(VOARMV6)
void Yuv2yuv_R90_inter_arm(VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride);
void Yuv2yuv_L90_inter_arm(VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride);
void Yuv2yuv_180_inter_arm(VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride);

#define Yuv2yuv_R90_inter            Yuv2yuv_R90_inter_arm
#define Yuv2yuv_L90_inter            Yuv2yuv_L90_inter_arm
#define Yuv2yuv_180_inter            Yuv2yuv_180_inter_arm
#elif defined(VOARMV7)
void Yuv2yuv_R90_inter_arm(VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride);
void Yuv2yuv_L90_inter_arm(VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride);
void Yuv2yuv_180_inter_arm(VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride);

#define Yuv2yuv_R90_inter Yuv2yuv_R90_inter_arm
#define Yuv2yuv_L90_inter Yuv2yuv_L90_inter_arm
#define Yuv2yuv_180_inter Yuv2yuv_180_inter_arm
#else
#define Yuv2yuv_R90_inter Yuv2yuv_R90_inter_c
#define Yuv2yuv_L90_inter Yuv2yuv_L90_inter_c
#define Yuv2yuv_180_inter Yuv2yuv_180_inter_c
#endif




#endif							/*_YUV_TO_YUV_H_ */
