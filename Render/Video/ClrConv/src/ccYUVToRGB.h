/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _YUV_TO_RGB_H_
#define _YUV_TO_RGB_H_

#include "voYYDef_CC.h"
#include "voColorConversion.h"
#include "ccInternal.h"


VOCCRETURNCODE YUVPlanarToRGB16_noresize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB32_noresize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToARGB32_noresize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanar444ToRGB16_noresize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanar422_21ToRGB16_noresize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanar411ToRGB16_noresize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanar411VToRGB16_noresize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB24_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB32_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToARGB32_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB24_noresize_rotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB32_noresize_rotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToARGB32_noresize_rotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV422InterlaceToRGB16_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV422PackedToYUV420Planar_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV422InterlaceToRGB24_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV422PlanarToYUV420Planar(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

VOCCRETURNCODE YUVPlanarToRGB16_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_noRotation_half(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_noRotation_3quarter(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_noRotation_1point5(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_noRotation_double(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_424X320_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanar444ToRGB16_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanar422_21ToRGB16_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanar411ToRGB16_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanar411VToRGB16_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB24_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV422InterlaceToRGB16_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV422InterlaceToRGB24_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_normal_flip(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

VOCCRETURNCODE YUVPlanarToRGB16_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_90L(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_half_90L(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_3quarter_90L(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_1point5_90L(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_double_90L(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_90R(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_half_90R(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_3quarter_90R(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_1point5_90R(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_double_90R(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

VOCCRETURNCODE YUVPlanarToRGB32_resize_norotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB32_resize_norotation_half(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB32_resize_rotation90l(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB32_resize_rotation90l_half(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB32_resize_rotation90r(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB32_resize_rotation90r_half(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToRGB32_resize_rotation180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

VOCCRETURNCODE YUVPlanarToARGB32_resize_norotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToARGB32_resize_norotation_half(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToARGB32_resize_rotation90l(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToARGB32_resize_rotation90l_half(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToARGB32_resize_rotation90r(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToARGB32_resize_rotation90r_half(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUVPlanarToARGB32_resize_rotation180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

VOCCRETURNCODE YUVPlanarToRGB16_antiAlias_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV444PlanarToRGB16_antiAlias_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV411PlanarToRGB16_antiAlias_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV411VPlanarToRGB16_antiAlias_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE YUV422_21PlanarToRGB16_antiAlias_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

extern CLRCONV_FUNC_PTR YUVPlanarToRGB16[2];
extern CLRCONV_FUNC_PTR YUVPlanarToRGB32ResizeRotation[7];
extern CLRCONV_FUNC_PTR YUVPlanarToARGB32ResizeRotation[7];
extern CLRCONV_FUNC_PTR YUVPlanarToRGB24[2];

extern CLRCONV_FUNC_PTR YUVPlanarToRGB16ResizeRotation[18];
extern CLRCONV_FUNC_PTR YUV444PlanarToRGB16ResizeNoRotation[2];
extern CLRCONV_FUNC_PTR YUV422_21PlanarToRGB16ResizeNoRotation[2];
extern CLRCONV_FUNC_PTR YUV411PlanarToRGB16ResizeNoRotation[2];
extern CLRCONV_FUNC_PTR YUV411VPlanarToRGB16ResizeNoRotation[2];

extern CLRCONV_FUNC_PTR YUV422InterlaceToRGB16[2];
extern CLRCONV_FUNC_PTR YUV422InterlaceToRGB24[2];

#ifdef VOX86
VOCCRETURNCODE MultiClrConv(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd);
#endif

#endif							/* _YUV_TO_RGB_H_ */
