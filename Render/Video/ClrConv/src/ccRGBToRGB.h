/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _RGB_TO_RGB_H_
#define _RGB_TO_RGB_H_

#include "voYYDef_CC.h"
#include "voColorConversion.h"
#include "ccInternal.h"

VOCCRETURNCODE RGB888PlanarToRGB16_noresize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE cc_rgb24_disable(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE Rgb16_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

VOCCRETURNCODE RGB888PlanarToRGB16_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE Rgb16_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE cc_s_rgb24_disable(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

VOCCRETURNCODE RGB888PlanarToRGB16_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE RGB888PlanarToRGB16_resize_Rotation_90L(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE RGB888PlanarToRGB16_resize_Rotation_90R(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE RGB888PlanarToRGB16_resize_Rotation_180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE Rgb16_resize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE Rgb16_noresize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE cc_rgb24_l90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE cc_rgb24_r90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE cc_s_rgb24_l90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE cc_s_rgb24_r90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

VOCCRETURNCODE RGB888PlanarToRGB16_antiAlias_resize_noRotation(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);
VOCCRETURNCODE rgb565_resize_anti_aliasing(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd);

#ifdef COLCONVENABLE
extern CLRCONV_FUNC_PTR RGB888PlanarToRGB16ResizeNoRotation[5];
extern CLRCONV_FUNC_PTR RGB888PlanarToRGB16[2];
#endif //COLCONVENABLE

extern CLRCONV_FUNC_PTR RGB24TORGB565[6];

#endif							/* _RGB_TO_RGB_H_ */
