/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#include <stdlib.h>

#include "ccYUVToRGBMB.h"
#include "ccYUVToRGB.h"
#include "ccConstant.h"


//{cConstV1, cConstV2, cConstU1, cConstU2}
VO_S32 param_tab[3][4] =
{
    {1471152, 748683, 1859125, 360710},//ITU-R BT 601
    {1646369, 489055, 1945737, 196083}
};//ITU-R BT 709

//VO_S32 *yuv2rgbmatrix;

VO_S32 ditherrb[2][2] =
{
    {RBDITHER_P0, RBDITHER_P1},
    {RBDITHER_P2, RBDITHER_P3}
};

VO_S32 ditherg[2][2] =
{
    {GDITHER_P0, GDITHER_P1},
    {GDITHER_P2, GDITHER_P3}
};

#ifdef COLCONVENABLE
static VO_U32 u_shift_tab[4] = {1, 3, 0, 2};
static VO_U32 v_shift_tab[4] = {3, 1, 2, 0};
#endif

#define DEBUG_DLL 0


CLRCONV_FUNC_PTR YUVPlanarToRGB16[2] =
{
    YUVPlanarToRGB16_noresize, YUVPlanarToRGB16_resize
};

CLRCONV_FUNC_PTR YUVPlanarToRGB32ResizeRotation[7] =
{
#ifndef VOX86
    YUVPlanarToRGB32_resize_norotation, YUVPlanarToRGB32_resize_norotation_half,
#else
    MultiClrConv, YUVPlanarToRGB32_resize_norotation_half,
#endif
    YUVPlanarToRGB32_resize_rotation90l, YUVPlanarToRGB32_resize_rotation90l_half,
    YUVPlanarToRGB32_resize_rotation90r, YUVPlanarToRGB32_resize_rotation90r_half,
    YUVPlanarToRGB32_resize_rotation180,
};

CLRCONV_FUNC_PTR YUVPlanarToARGB32ResizeRotation[7] =
{
#ifndef VOX86
    YUVPlanarToARGB32_resize_norotation, YUVPlanarToARGB32_resize_norotation_half,
#else
    MultiClrConv, YUVPlanarToARGB32_resize_norotation_half,
#endif
    YUVPlanarToARGB32_resize_rotation90l, YUVPlanarToARGB32_resize_rotation90l_half,
    YUVPlanarToARGB32_resize_rotation90r, YUVPlanarToARGB32_resize_rotation90r_half,
    YUVPlanarToARGB32_resize_rotation180,
};

CLRCONV_FUNC_PTR YUVPlanarToRGB16ResizeRotation[18] =
{
    YUVPlanarToRGB16_resize_noRotation, YUVPlanarToRGB16_resize_noRotation_half, YUVPlanarToRGB16_resize_noRotation_3quarter, YUVPlanarToRGB16_resize_noRotation_1point5, YUVPlanarToRGB16_resize_noRotation_double,
    YUVPlanarToRGB16_resize_Rotation_90L, YUVPlanarToRGB16_resize_Rotation_half_90L, YUVPlanarToRGB16_resize_Rotation_3quarter_90L, YUVPlanarToRGB16_resize_Rotation_1point5_90L, YUVPlanarToRGB16_resize_Rotation_double_90L,
    YUVPlanarToRGB16_resize_Rotation_90R, YUVPlanarToRGB16_resize_Rotation_half_90R, YUVPlanarToRGB16_resize_Rotation_3quarter_90R, YUVPlanarToRGB16_resize_Rotation_1point5_90R, YUVPlanarToRGB16_resize_Rotation_double_90R,
    YUVPlanarToRGB16_resize_Rotation_180, YUVPlanarToRGB16_antiAlias_resize_noRotation, YUVPlanarToRGB16_resize_424X320_noRotation,
};

static const CC_MB_PTR cc_mb_all[2] =
{
    cc_mb, cc_mb_16x16
};


#ifndef VOARMV7
static const CC_MB_PTR cc_rgb32_mb_all[2] =
{
    cc_rgb32_mb, cc_rgb32_mb_16x16
};
#else
static const CC_MB_PTR cc_rgb32_mb_all[2] =
{
    cc_rgb32_mb_new, cc_rgb32_neon
};
#endif

#ifndef VOARMV7
static const CC_MB_PTR cc_argb32_mb_all[2] =
{
    cc_argb32_mb, cc_argb32_mb_16x16
};
#else
static const CC_MB_PTR cc_argb32_mb_all[2] =
{
    cc_argb32_mb_new, cc_argb32_neon
};
#endif

CLRCONV_FUNC_PTR YUVPlanarToRGB24[2] =
{
    YUVPlanarToRGB24_noresize, YUVPlanarToRGB24_resize
};

static const CC_MB_PTR yuv420_rgb24_mb[2] =
{
    yuv420_rgb24, yuv420_rgb24_mb_16x16
};

#ifdef COLCONVENABLE
CLRCONV_FUNC_PTR YUV422InterlaceToRGB16[2] =
{
    //RGB16
    YUV422InterlaceToRGB16_noresize, YUV422InterlaceToRGB16_resize
};

CLRCONV_FUNC_PTR YUV422InterlaceToRGB24[2] =
{
    //RGB24
    YUV422InterlaceToRGB24_noresize, YUV422InterlaceToRGB24_resize
};

YUV422INTERLACETORGB16_MB_FUNC_PTR Yuv422Interlace_mb[8][4] =
{
    {YUYVToRGB16_MB_rotation_no, YUYVToRGB16_MB_rotation_90L, YUYVToRGB16_MB_rotation_90R, YUYVToRGB16_MB_rotation_180},
    {YVYUToRGB16_MB_rotation_no, YVYUToRGB16_MB_rotation_90L, YVYUToRGB16_MB_rotation_90R, YVYUToRGB16_MB_rotation_180},
    {UYVYToRGB16_MB_rotation_no, UYVYToRGB16_MB_rotation_90L, UYVYToRGB16_MB_rotation_90R, UYVYToRGB16_MB_rotation_180},
    {VYUYToRGB16_MB_rotation_no, VYUYToRGB16_MB_rotation_90L, VYUYToRGB16_MB_rotation_90R, VYUYToRGB16_MB_rotation_180},
    {YUYV2ToRGB16_MB_rotation_no, YUYV2ToRGB16_MB_rotation_90L, YUYV2ToRGB16_MB_rotation_90R, YUYV2ToRGB16_MB_rotation_180},
    {YVYU2ToRGB16_MB_rotation_no, YVYU2ToRGB16_MB_rotation_90L, YVYU2ToRGB16_MB_rotation_90R, YVYU2ToRGB16_MB_rotation_180},
    {UYVY2ToRGB16_MB_rotation_no, UYVY2ToRGB16_MB_rotation_90L, UYVY2ToRGB16_MB_rotation_90R, UYVY2ToRGB16_MB_rotation_180},
    {VYUY2ToRGB16_MB_rotation_no, VYUY2ToRGB16_MB_rotation_90L, VYUY2ToRGB16_MB_rotation_90R, VYUY2ToRGB16_MB_rotation_180}
};

CLRCONV_FUNC_PTR YUV444PlanarToRGB16ResizeNoRotation[2] =
{
    YUVPlanar444ToRGB16_resize_noRotation, YUV444PlanarToRGB16_antiAlias_resize_noRotation
};

CLRCONV_FUNC_PTR YUV422_21PlanarToRGB16ResizeNoRotation[2] =
{
    YUVPlanar422_21ToRGB16_resize_noRotation, YUV422_21PlanarToRGB16_antiAlias_resize_noRotation
};

CLRCONV_FUNC_PTR YUV411PlanarToRGB16ResizeNoRotation[2] =
{
    YUVPlanar411ToRGB16_resize_noRotation, YUV411PlanarToRGB16_antiAlias_resize_noRotation
};

CLRCONV_FUNC_PTR YUV411VPlanarToRGB16ResizeNoRotation[2] =
{
    YUVPlanar411VToRGB16_resize_noRotation, YUV411VPlanarToRGB16_antiAlias_resize_noRotation
};

static const CC_MB_NEW_PTR YUV444_to_RGB565_mb_all[2] =
{
    YUV444_to_RGB565_cc_mb, YUV444_to_RGB565_cc_mb_16x16
};

static const CC_MB_NEW_PTR YUV422_21_to_RGB565_mb_all[2] =
{
    YUV422_21_to_RGB565_cc_mb, YUV422_21_to_RGB565_cc_mb_16x16
};

static const CC_MB_NEW_PTR YUV411_to_RGB565_mb_all[2] =
{
    YUV411_to_RGB565_cc_mb, YUV411_to_RGB565_cc_mb_16x16
};

static const CC_MB_NEW_PTR YUV411V_to_RGB565_mb_all[2] =
{
    YUV411V_to_RGB565_cc_mb, YUV411V_to_RGB565_cc_mb_16x16
};
#endif

#if DEBUG_DLL
void DEBUG(VO_U8 *str)
{
    do
    {
        FILE *f;
        f = fopen("D:/cc.log", "a+");
        fputs(str, f);
        fclose(f);
    }
    while(0);
}
#endif

VOCCRETURNCODE YUVPlanarToRGB16_noresize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_S32 vx = 0, width, height;
    VO_U8 *psrc_y = conv_data->pInBuf[0];
    VO_U8 *psrc_u = conv_data->pInBuf[1];
    VO_U8 *psrc_v = conv_data->pInBuf[2];
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_U8	*start_out_buf, *out_buf;
    VO_S32 skip = 0;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 out_width = conv_data->nOutWidth;
    VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    mb_skip = conv_data->mb_skip;

    y = psrc_y;
    u = psrc_u;
    v = psrc_v;
    //add end

    start_out_buf = out_buf = conv_data->pOutBuf[0];

    do
    {
        vx = out_width;
        out_buf = start_out_buf;
        do
        {
            if(mb_skip) skip = *(mb_skip++);

            if(!skip)
            {
                width = vx < 16 ? vx : 16;
                height = out_height < 16 ? out_height : 16;
                cc_mb_all[(width==16)&&(height==16)](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width, height, uin_stride, vin_stride);
            }

            psrc_y += 16;
            psrc_u += 8;
            psrc_v += 8;
            out_buf += 32;
        }
        while((vx -= 16) > 0);

        psrc_y = y = y + (in_stride << 4);
        psrc_u = u = u + (uin_stride << 3);
        psrc_v = v = v + (vin_stride << 3);
        start_out_buf += (out_stride << 4);
    }
    while((out_height -= 16) > 0);

    return VO_ERR_NONE;

}

VOCCRETURNCODE YUVPlanarToRGB32_noresize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_S32 vx = 0, width, height;
    VO_U8 *psrc_y = conv_data->pInBuf[0];
    VO_U8 *psrc_u = conv_data->pInBuf[1];
    VO_U8 *psrc_v = conv_data->pInBuf[2];
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_U8	*start_out_buf, *out_buf;
    VO_S32 skip = 0;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 out_width = conv_data->nOutWidth;
    VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    mb_skip = conv_data->mb_skip;

    y = psrc_y;
    u = psrc_u;
    v = psrc_v;
    //add end

    start_out_buf = out_buf = conv_data->pOutBuf[0];
#ifdef VOARMV7
    //out_height must 2n
    cc_rgb32_mb_all[1](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, out_width - (out_width & 0xf), out_height, uin_stride, vin_stride);

    if((out_width & 0xf)) //width remain
    {
        int width_remain = (out_width & 0xf);
        int width_height = out_height;
        psrc_y += out_width - width_remain;
        psrc_u += (out_width - width_remain) >> 1;
        psrc_v += (out_width - width_remain) >> 1;
        out_buf += (out_width - width_remain) * 4;
        cc_rgb32_mb_all[0](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width_remain, out_height, uin_stride, vin_stride);
    }
#else

    do
    {
        vx = out_width;
        out_buf = start_out_buf;
        do
        {
            if(mb_skip) skip = *(mb_skip++);

            if(!skip)
            {
                width = vx < 16 ? vx : 16;
                height = out_height < 16 ? out_height : 16;
                cc_rgb32_mb_all[(width==16)&&(height==16)](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width, height, uin_stride, vin_stride);
            }

            psrc_y += 16;
            psrc_u += 8;
            psrc_v += 8;
            out_buf += 64;
        }
        while((vx -= 16) > 0);

        psrc_y = y = y + (in_stride << 4);
        psrc_u = u = u + (uin_stride << 3);
        psrc_v = v = v + (vin_stride << 3);
        start_out_buf += (out_stride << 4);
    }
    while((out_height -= 16) > 0);
#endif
    return VO_ERR_NONE;

}


VOCCRETURNCODE YUVPlanarToRGB32_noresize_rotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U32 vx = 0, vy = 0;
    const VO_U32 in_stride = conv_data->nInStride, out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;
    VO_U8 *psrc_y, *psrc_u, *psrc_v, *out_buf, *start_out_buf = NULL;
    //VO_S32 *param = cc_hnd->yuv2rgbmatrix;
    VO_S32 step_x = 0, step_y = 0;
    const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);
#if defined(VOARMV7)
    VO_U32 uin_stride = conv_data->nInStride >> is420Planar;
    VO_U32 vin_stride = conv_data->nInStride >> is420Planar;
    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;

    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;

    if((out_width < 16) || (out_height < 16))
        goto Normal_convert;

    if(conv_data->nRotationType == ROTATION_90L)
    {
        YUV420PLANARTOYUV420_MB_ROTATION = cc_yuv2yuv_8x8_L90_armv7;
        start_out_buf = conv_data->pOutBuf[0] + (out_width - 8) * out_stride;
        step_x = -(out_stride << 3);
        step_y = 32;
    }
    else if(conv_data->nRotationType == ROTATION_90R)
    {
        YUV420PLANARTOYUV420_MB_ROTATION = cc_yuv2yuv_8x8_R90_armv7;
        start_out_buf = conv_data->pOutBuf[0] + (out_height << 2) - 32;
        step_x = (out_stride << 3);
        step_y = -32;

    }
    else if(conv_data->nRotationType == ROTATION_180)
    {
        YUV420PLANARTOYUV420_MB_ROTATION = cc_yuv2yuv_8x8_R180_armv7;
        start_out_buf = conv_data->pOutBuf[0] + out_stride * (conv_data->nOutHeight - 7) - 32;
        step_x = -32;
        step_y = -(out_stride << 3);
    }

    /*prepare resize*/
    for(vy = 8; vy <= out_height; vy += 8)
    {
        VO_S32 offset = (vy - 8) * in_stride;

        psrc_y = conv_data->pInBuf[0] + offset;
        psrc_u = conv_data->pInBuf[1] + (offset >> (1 + is420Planar));
        psrc_v = conv_data->pInBuf[2] + (offset >> (1 + is420Planar));

        out_buf = start_out_buf;
        for(vx = 8; vx <= out_width; vx += 8)
        {

            YUV420PLANARTOYUV420_MB_ROTATION(psrc_y, psrc_u, psrc_v, in_stride, uin_stride, vin_stride, dsty, dstu, dstv);
            voyuv420torgb32_8nx2n_armv7(dsty, dstu, dstv, 8, out_buf, out_stride, 8, 8, 4, 4);

            psrc_y += 8;
            psrc_u += 4;
            psrc_v += 4;
            out_buf += step_x;

        }
        start_out_buf += step_y;
    }
#else
    VO_U8 *mb_skip;
    VO_S32 skip = 0;
    VO_U32 uin_stride = conv_data->nInUVStride << (!is420Planar);
    VO_U32 vin_stride = conv_data->nInUVStride << (!is420Planar);

    mb_skip = conv_data->mb_skip;

    if((out_width < 16) || (out_height < 16))
        goto Normal_convert;

    if(conv_data->nRotationType == ROTATION_90L)
    {
        YUV420PLANARTORGB32_NEWMB = YUV420PlanarToRGB32_NEWMB_rotation_90L;
        start_out_buf = conv_data->pOutBuf[0] + (out_width - 1) * out_stride;
        step_x = -(out_stride << 4);
        step_y = 64;
    }
    else if(conv_data->nRotationType == ROTATION_90R)
    {
        YUV420PLANARTORGB32_NEWMB = YUV420PlanarToRGB32_NEWMB_rotation_90R;
        start_out_buf = conv_data->pOutBuf[0] + (out_height << 2) - 8;
        step_x = (out_stride << 4);
        step_y = -64;
    }
    else if(conv_data->nRotationType == ROTATION_180)
    {
        YUV420PLANARTORGB32_NEWMB = YUV420PlanarToRGB32_NEWMB_rotation_180;
        start_out_buf = conv_data->pOutBuf[0] + (out_height - 15) * out_stride - 64;
        step_x = -64;
        step_y = -(out_stride << 4);
    }

    for(vy = 16; vy <= out_height; vy += 16)
    {
        VO_S32 offset = (vy - 16) * in_stride;

        psrc_y = conv_data->pInBuf[0] + offset;
        offset = ((vy - 16) >> 1) * uin_stride;
        psrc_u = conv_data->pInBuf[1] + offset;
        psrc_v = conv_data->pInBuf[2] + offset;

        out_buf = start_out_buf;
        for(vx = 16; vx <= out_width; vx += 16)
        {
            if(mb_skip) skip = *(mb_skip++);

            if(!skip)
            {
                YUV420PLANARTORGB32_NEWMB(psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, 16, 16, uin_stride, vin_stride);
            }

            psrc_y += 16;
            psrc_u += 8;
            psrc_v += 8;
            out_buf += step_x;

        }
        start_out_buf += step_y;
    }
#endif

#if defined(VOARMV7)
    vx = vx - 8;
    vy = vy - 8;
#else
    vx = vx - 16;
    vy = vy - 16;
#endif

Normal_convert:

    if(vx != out_width) /*not multiple of 16*/
    {

        ClrConvData tmp_conv_data;
        tmp_conv_data = *conv_data;

        tmp_conv_data.nOutWidth = out_width - vx;
        tmp_conv_data.nOutHeight = vy;

        tmp_conv_data.pInBuf[0] = conv_data->pInBuf[0] + vx;
        tmp_conv_data.pInBuf[1] = conv_data->pInBuf[1] + (vx >> 1);
        tmp_conv_data.pInBuf[2] = conv_data->pInBuf[2] + (vx >> 1);
        if(conv_data->nRotationType == ROTATION_90L)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0];
        }
        else if(conv_data->nRotationType == ROTATION_90R)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + out_stride * (vx + 1) - (vy << 2);
        }
        else if(conv_data->nRotationType == ROTATION_180)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + ((out_width - vx) << 2) + out_stride * (out_height - 1);
        }
        else
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (vx << 2);
        }

        NewYUVPlanarToRGB32_normal(&tmp_conv_data, isRGB565, is420Planar, cc_hnd);
    }

    if(vy != out_height)
    {
        ClrConvData tmp_conv_data;
        tmp_conv_data = *conv_data;

        tmp_conv_data.pInBuf[0] = conv_data->pInBuf[0] + vy * in_stride;
        tmp_conv_data.pInBuf[1] = conv_data->pInBuf[1] + (vy >> is420Planar) * (in_stride >> 1);
        tmp_conv_data.pInBuf[2] = conv_data->pInBuf[2] + (vy >> is420Planar) * (in_stride >> 1);

        if(conv_data->nRotationType == ROTATION_90L)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (vy << 2);
        }
        else if(conv_data->nRotationType == ROTATION_90R)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0];
        }
        else if(conv_data->nRotationType == ROTATION_180)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (out_height - vy) * out_stride;
        }
        else
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + vy * out_stride;
        }

        tmp_conv_data.nOutWidth = out_width;
        tmp_conv_data.nOutHeight = out_height - vy;
        NewYUVPlanarToRGB32_normal(&tmp_conv_data, isRGB565, is420Planar, cc_hnd);
    }

    return VO_ERR_NONE;
}
VOCCRETURNCODE YUVPlanarToRGB32_noresize(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    if (ROTATION_DISABLE == conv_data->nRotationType)
    {
#ifdef VOX86
		MultiClrConv(conv_data, x_resize_tab, y_resize_tab, cc_hnd);
#else
        YUVPlanarToRGB32_noresize_noRotation(conv_data, x_resize_tab, y_resize_tab, cc_hnd);
#endif
    }
    else
    {
        YUVPlanarToRGB32_noresize_rotation(conv_data, x_resize_tab, y_resize_tab, cc_hnd);
    }
    return VO_ERR_NONE;
}



VOCCRETURNCODE YUVPlanarToARGB32_noresize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_S32 vx = 0, width, height;
    VO_U8 *psrc_y = conv_data->pInBuf[0];
    VO_U8 *psrc_u = conv_data->pInBuf[1];
    VO_U8 *psrc_v = conv_data->pInBuf[2];
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_U8	*start_out_buf, *out_buf;
    VO_S32 skip = 0;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 out_width = conv_data->nOutWidth;
    VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    mb_skip = conv_data->mb_skip;

    y = psrc_y;
    u = psrc_u;
    v = psrc_v;
    //add end

    start_out_buf = out_buf = conv_data->pOutBuf[0];
#ifdef VOARMV7

    //out_height must 2n
    cc_argb32_mb_all[1](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, out_width - (out_width & 0xf), out_height, uin_stride, vin_stride);

    if((out_width & 0xf)) //width remain
    {
        int width_remain = (out_width & 0xf);
        int width_height = out_height;
        psrc_y += out_width - width_remain;
        psrc_u += (out_width - width_remain) >> 1;
        psrc_v += (out_width - width_remain) >> 1;
        out_buf += (out_width - width_remain) * 4;
        cc_argb32_mb_all[0](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width_remain, out_height, uin_stride, vin_stride);
    }
#else
    do
    {
        vx = out_width;
        out_buf = start_out_buf;
        do
        {
            if(mb_skip) skip = *(mb_skip++);

            if(!skip)
            {
                width = vx < 16 ? vx : 16;
                height = out_height < 16 ? out_height : 16;
#ifdef VOX86
                voyuv420toargb32_16nx2n_x86(psrc_y, psrc_u, psrc_v, in_stride, uin_stride, width, height, out_buf, out_stride / 4);
#else
    cc_argb32_mb_all[(width==16)&&(height==16)](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width, height, uin_stride, vin_stride);
#endif
            }

            psrc_y += 16;
            psrc_u += 8;
            psrc_v += 8;
            out_buf += 64;
        }
        while((vx -= 16) > 0);

        psrc_y = y = y + (in_stride << 4);
        psrc_u = u = u + (uin_stride << 3);
        psrc_v = v = v + (vin_stride << 3);
        start_out_buf += (out_stride << 4);
    }
    while((out_height -= 16) > 0);

#endif
    return VO_ERR_NONE;

}


VOCCRETURNCODE YUVPlanarToARGB32_noresize_rotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U32 vx = 0, vy = 0;
    const VO_U32 in_stride = conv_data->nInStride, out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;
    VO_U8 *psrc_y, *psrc_u, *psrc_v, *out_buf, *start_out_buf = NULL;
    //VO_S32 *param = cc_hnd->yuv2rgbmatrix;
    VO_S32 step_x = 0, step_y = 0;
    const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);
#if defined(VOARMV7)
    VO_U32 uin_stride = conv_data->nInStride >> is420Planar;
    VO_U32 vin_stride = conv_data->nInStride >> is420Planar;
    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;

    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;

    if((out_width < 16) || (out_height < 16))
        goto Normal_convert;

    if(conv_data->nRotationType == ROTATION_90L)
    {
        YUV420PLANARTOYUV420_MB_ROTATION = cc_yuv2yuv_8x8_L90_armv7;
        start_out_buf = conv_data->pOutBuf[0] + (out_width - 8) * out_stride;
        step_x = -(out_stride << 3);
        step_y = 32;
    }
    else if(conv_data->nRotationType == ROTATION_90R)
    {
        YUV420PLANARTOYUV420_MB_ROTATION = cc_yuv2yuv_8x8_R90_armv7;
        start_out_buf = conv_data->pOutBuf[0] + (out_height << 2) - 32;
        step_x = (out_stride << 3);
        step_y = -32;

    }
    else if(conv_data->nRotationType == ROTATION_180)
    {
        YUV420PLANARTOYUV420_MB_ROTATION = cc_yuv2yuv_8x8_R180_armv7;
        start_out_buf = conv_data->pOutBuf[0] + out_stride * (conv_data->nOutHeight - 7) - 32;
        step_x = -32;
        step_y = -(out_stride << 3);
    }

    /*prepare resize*/
    for(vy = 8; vy <= out_height; vy += 8)
    {
        VO_S32 offset = (vy - 8) * in_stride;

        psrc_y = conv_data->pInBuf[0] + offset;
        psrc_u = conv_data->pInBuf[1] + (offset >> (1 + is420Planar));
        psrc_v = conv_data->pInBuf[2] + (offset >> (1 + is420Planar));

        out_buf = start_out_buf;
        for(vx = 8; vx <= out_width; vx += 8)
        {

            YUV420PLANARTOYUV420_MB_ROTATION(psrc_y, psrc_u, psrc_v, in_stride, uin_stride, vin_stride, dsty, dstu, dstv);
            voyuv420toargb32_8nx2n_armv7(dsty, dstu, dstv, 8, out_buf, out_stride, 8, 8, 4, 4);

            psrc_y += 8;
            psrc_u += 4;
            psrc_v += 4;
            out_buf += step_x;

        }
        start_out_buf += step_y;
    }
#else
    VO_U8 *mb_skip;
    VO_S32 skip = 0;
    VO_U32 uin_stride = conv_data->nInUVStride << (!is420Planar);
    VO_U32 vin_stride = conv_data->nInUVStride << (!is420Planar);

    mb_skip = conv_data->mb_skip;

    if((out_width < 16) || (out_height < 16))
        goto Normal_convert;

    if(conv_data->nRotationType == ROTATION_90L)
    {
        YUV420PLANARTOARGB32_NEWMB = YUV420PlanarToARGB32_NEWMB_rotation_90L;
        start_out_buf = conv_data->pOutBuf[0] + (out_width - 1) * out_stride;
        step_x = -(out_stride << 4);
        step_y = 64;
    }
    else if(conv_data->nRotationType == ROTATION_90R)
    {
        YUV420PLANARTOARGB32_NEWMB = YUV420PlanarToARGB32_NEWMB_rotation_90R;
        start_out_buf = conv_data->pOutBuf[0] + (out_height << 2) - 8;
        step_x = (out_stride << 4);
        step_y = -64;
    }
    else if(conv_data->nRotationType == ROTATION_180)
    {
        YUV420PLANARTOARGB32_NEWMB = YUV420PlanarToARGB32_NEWMB_rotation_180;
        start_out_buf = conv_data->pOutBuf[0] + (out_height - 15) * out_stride - 64;
        step_x = -64;
        step_y = -(out_stride << 4);
    }

    for(vy = 16; vy <= out_height; vy += 16)
    {
        VO_S32 offset = (vy - 16) * in_stride;

        psrc_y = conv_data->pInBuf[0] + offset;
        offset = ((vy - 16) >> 1) * uin_stride;
        psrc_u = conv_data->pInBuf[1] + offset;
        psrc_v = conv_data->pInBuf[2] + offset;

        out_buf = start_out_buf;
        for(vx = 16; vx <= out_width; vx += 16)
        {
            if(mb_skip) skip = *(mb_skip++);

            if(!skip)
            {
                YUV420PLANARTOARGB32_NEWMB(psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, 16, 16, uin_stride, vin_stride);
            }

            psrc_y += 16;
            psrc_u += 8;
            psrc_v += 8;
            out_buf += step_x;

        }
        start_out_buf += step_y;
    }
#endif

#if defined(VOARMV7)
    vx = vx - 8;
    vy = vy - 8;
#else
    vx = vx - 16;
    vy = vy - 16;
#endif

Normal_convert:

    if(vx != out_width) /*not multiple of 16*/
    {

        ClrConvData tmp_conv_data;
        tmp_conv_data = *conv_data;

        tmp_conv_data.nOutWidth = out_width - vx;
        tmp_conv_data.nOutHeight = vy;

        tmp_conv_data.pInBuf[0] = conv_data->pInBuf[0] + vx;
        tmp_conv_data.pInBuf[1] = conv_data->pInBuf[1] + (vx >> 1);
        tmp_conv_data.pInBuf[2] = conv_data->pInBuf[2] + (vx >> 1);
        if(conv_data->nRotationType == ROTATION_90L)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0];
        }
        else if(conv_data->nRotationType == ROTATION_90R)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + out_stride * (vx + 1) - (vy << 2);
        }
        else if(conv_data->nRotationType == ROTATION_180)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + ((out_width - vx) << 2) + out_stride * (out_height - 1);
        }
        else
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (vx << 2);
        }

        NewYUVPlanarToARGB32_normal(&tmp_conv_data, isRGB565, is420Planar, cc_hnd);
    }

    if(vy != out_height)
    {
        ClrConvData tmp_conv_data;
        tmp_conv_data = *conv_data;

        tmp_conv_data.pInBuf[0] = conv_data->pInBuf[0] + vy * in_stride;
        tmp_conv_data.pInBuf[1] = conv_data->pInBuf[1] + (vy >> is420Planar) * (in_stride >> 1);
        tmp_conv_data.pInBuf[2] = conv_data->pInBuf[2] + (vy >> is420Planar) * (in_stride >> 1);

        if(conv_data->nRotationType == ROTATION_90L)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (vy << 2);
        }
        else if(conv_data->nRotationType == ROTATION_90R)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0];
        }
        else if(conv_data->nRotationType == ROTATION_180)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (out_height - vy) * out_stride;
        }
        else
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + vy * out_stride;
        }

        tmp_conv_data.nOutWidth = out_width;
        tmp_conv_data.nOutHeight = out_height - vy;
        NewYUVPlanarToARGB32_normal(&tmp_conv_data, isRGB565, is420Planar, cc_hnd);
    }

    return VO_ERR_NONE;
}
VOCCRETURNCODE YUVPlanarToARGB32_noresize(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    if (ROTATION_DISABLE == conv_data->nRotationType)
    {
#ifdef VOX86
        MultiClrConv(conv_data, x_resize_tab, y_resize_tab, cc_hnd);
#else
        YUVPlanarToARGB32_noresize_noRotation(conv_data, x_resize_tab, y_resize_tab, cc_hnd);
#endif
    }
    else
    {
        YUVPlanarToARGB32_noresize_rotation(conv_data, x_resize_tab, y_resize_tab, cc_hnd);
    }
    return VO_ERR_NONE;
}

static VOCCRETURNCODE YUVPlanarToRGB24_noresize_norotation(ClrConvData *conv_data, const VO_U32 is420Planar,
        VO_S32 *x_resize_tab, VO_S32 *y_resize_tab)
{
    VO_S32 vx = 0, width, height;
    VO_U8 *psrc_y = conv_data->pInBuf[0];
    VO_U8 *psrc_u = conv_data->pInBuf[1];
    VO_U8 *psrc_v = conv_data->pInBuf[2];
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_U8	*start_out_buf, *out_buf;
    VO_S32 skip = 0;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 out_width = conv_data->nOutWidth;
    VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    mb_skip = conv_data->mb_skip;

    y = psrc_y;
    u = psrc_u;
    v = psrc_v;
    //add end

    start_out_buf = out_buf = conv_data->pOutBuf[0];

    do
    {
        vx = out_width;
        out_buf = start_out_buf;
        do
        {
            if(mb_skip) skip = *(mb_skip++);

            if(!skip)
            {
                width = vx < 16 ? vx : 16;
                height = out_height < 16 ? out_height : 16;
                yuv420_rgb24_mb[(width==16)&&(height==16)](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width, height, uin_stride, vin_stride);
            }

            psrc_y += 16;
            psrc_u += 8;
            psrc_v += 8;
            out_buf += 48;
        }
        while((vx -= 16) > 0);

        psrc_y = y = y + (in_stride << 4);
        psrc_u = u = u + (uin_stride << 3);
        psrc_v = v = v + (vin_stride << 3);
        start_out_buf += (out_stride << 4);
    }
    while((out_height -= 16) > 0);

    return VO_ERR_NONE;

}

VOCCRETURNCODE YUVPlanarToRGB24_noresize(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    YUVPlanarToRGB24_noresize_norotation(conv_data, is420Planar, x_resize_tab, y_resize_tab);

    return VO_ERR_NONE;
}

#ifdef COLCONVENABLE
VOCCRETURNCODE YUVPlanar444ToRGB16_noresize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_S32 vx = 0, width, height;
    VO_U8 *psrc_y = conv_data->pInBuf[0];
    VO_U8 *psrc_u = conv_data->pInBuf[1];
    VO_U8 *psrc_v = conv_data->pInBuf[2];
    VO_U8 *y, *u, *v;
    VO_U8	*start_out_buf, *out_buf;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 out_width = conv_data->nOutWidth;
    VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    //const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);
    VO_S32 *param = cc_hnd->yuv2rgbmatrix;

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInStride;
    vin_stride = conv_data->nInStride;

    y = psrc_y;
    u = psrc_u;
    v = psrc_v;
    //add end

    start_out_buf = out_buf = conv_data->pOutBuf[0];

    do
    {
        vx = out_width;
        out_buf = start_out_buf;
        do
        {

            width = vx < 16 ? vx : 16;
            height = out_height < 16 ? out_height : 16;
            YUV444_to_RGB565_mb_all[(width==16)&&(height==16)](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride,
                    width, height, uin_stride, vin_stride, param);

            psrc_y += 16;
            psrc_u += 16;
            psrc_v += 16;
            out_buf += 32;
        }
        while((vx -= 16) > 0);

        psrc_y = y = y + (in_stride  << 4);
        psrc_u = u = u + (uin_stride << 4);
        psrc_v = v = v + (vin_stride << 4);
        start_out_buf += (out_stride << 4);
    }
    while((out_height -= 16) > 0);

    return VO_ERR_NONE;

}

VOCCRETURNCODE YUVPlanar422_21ToRGB16_noresize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_S32 vx = 0, width, height;
    VO_U8 *psrc_y = conv_data->pInBuf[0];
    VO_U8 *psrc_u = conv_data->pInBuf[1];
    VO_U8 *psrc_v = conv_data->pInBuf[2];
    VO_U8 *y, *u, *v;
    VO_U8	*start_out_buf, *out_buf;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 out_width = conv_data->nOutWidth;
    VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    //const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);
    VO_S32 *param = cc_hnd->yuv2rgbmatrix;

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInStride;
    vin_stride = conv_data->nInStride;

    y = psrc_y;
    u = psrc_u;
    v = psrc_v;
    //add end

    start_out_buf = out_buf = conv_data->pOutBuf[0];

    do
    {
        vx = out_width;
        out_buf = start_out_buf;
        do
        {

            width = vx < 16 ? vx : 16;
            height = out_height < 16 ? out_height : 16;
            YUV422_21_to_RGB565_mb_all[(width==16)&&(height==16)](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride,
                    width, height, uin_stride, vin_stride, param);

            psrc_y += 16;
            psrc_u += 16;
            psrc_v += 16;
            out_buf += 32;
        }
        while((vx -= 16) > 0);

        psrc_y = y = y + (in_stride  << 4);
        psrc_u = u = u + (uin_stride << 3);
        psrc_v = v = v + (vin_stride << 3);
        start_out_buf += (out_stride << 4);
    }
    while((out_height -= 16) > 0);

    return VO_ERR_NONE;

}

VOCCRETURNCODE YUVPlanar411ToRGB16_noresize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_S32 vx = 0, width, height;
    VO_U8 *psrc_y = conv_data->pInBuf[0];
    VO_U8 *psrc_u = conv_data->pInBuf[1];
    VO_U8 *psrc_v = conv_data->pInBuf[2];
    VO_U8 *y, *u, *v;
    VO_U8	*start_out_buf, *out_buf;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 out_width = conv_data->nOutWidth;
    VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    //const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);
    VO_S32 *param = cc_hnd->yuv2rgbmatrix;

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInStride >> 2;
    vin_stride = conv_data->nInStride >> 2;

    y = psrc_y;
    u = psrc_u;
    v = psrc_v;
    //add end

    start_out_buf = out_buf = conv_data->pOutBuf[0];

    do
    {
        vx = out_width;
        out_buf = start_out_buf;
        do
        {

            width = vx < 16 ? vx : 16;
            height = out_height < 16 ? out_height : 16;
            YUV411_to_RGB565_mb_all[(width==16)&&(height==16)](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride,
                    width, height, uin_stride, vin_stride, param);

            psrc_y += 16;
            psrc_u += 4;
            psrc_v += 4;
            out_buf += 32;
        }
        while((vx -= 16) > 0);

        psrc_y = y = y + (in_stride  << 4);
        psrc_u = u = u + (uin_stride << 4);
        psrc_v = v = v + (vin_stride << 4);
        start_out_buf += (out_stride << 4);
    }
    while((out_height -= 16) > 0);

    return VO_ERR_NONE;

}

VOCCRETURNCODE YUVPlanar411VToRGB16_noresize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_S32 vx = 0, width, height;
    VO_U8 *psrc_y = conv_data->pInBuf[0];
    VO_U8 *psrc_u = conv_data->pInBuf[1];
    VO_U8 *psrc_v = conv_data->pInBuf[2];
    VO_U8 *y, *u, *v;
    VO_U8	*start_out_buf, *out_buf;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 out_width = conv_data->nOutWidth;
    VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    //const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);
    VO_S32 *param = cc_hnd->yuv2rgbmatrix;

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInStride;
    vin_stride = conv_data->nInStride;

    y = psrc_y;
    u = psrc_u;
    v = psrc_v;
    //add end

    start_out_buf = out_buf = conv_data->pOutBuf[0];

    //out_height = 16;

    do
    {
        vx = out_width;
        out_buf = start_out_buf;
        do
        {

            width = vx < 16 ? vx : 16;
            height = out_height < 16 ? out_height : 16;
            YUV411V_to_RGB565_mb_all[(width==16)&&(height==16)](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride,
                    width, height, uin_stride, vin_stride, param);

            psrc_y += 16;
            psrc_u += 16;
            psrc_v += 16;
            out_buf += 32;
        }
        while((vx -= 16) > 0);

        psrc_y = y = y + (in_stride  << 4);
        psrc_u = u = u + (uin_stride << 2);
        psrc_v = v = v + (vin_stride << 2);
        start_out_buf += (out_stride << 4);
    }
    while((out_height -= 16) > 0);

    return VO_ERR_NONE;

}


#if 0
static VOCCRETURNCODE YUVPlanarToRGB24_noresize_FLIPY(ClrConvData *conv_data, const VO_U32 is420Planar,
        VO_S32 *x_resize_tab, VO_S32 *y_resize_tab)
{
    VO_U32 vx = 0, vy;
    const VO_U32 in_stride = conv_data->nInStride , out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    const VO_S32 *param = yuv2rgbmatrix;
    VO_U8 *outbuf, *start_out_buf;
    VO_U8 *psrcY, *psrcU, *psrcV;

    VO_S32 out_stride = conv_data->nOutStride;
    VO_S32 step_y;

    step_y = -out_stride;
    start_out_buf = conv_data->pOutBuf[0] + ((out_height - 1) * out_stride);


    for(vy = 0; vy < out_height; vy += 2)
    {
        outbuf = start_out_buf;

        vx =  vy * in_stride;
        psrcY = conv_data->pInBuf[0] + vx;

        psrcU = conv_data->pInBuf[1] + (vx >> (2 - (!is420Planar)));
        psrcV = conv_data->pInBuf[2] + (vx >> (2 - (!is420Planar)));

        for(vx = 0; vx < out_width;)
        {
            VO_S32 a0, a1, a2, a3, a4;
            a4 = psrcY[vx];
            a3 = psrcV[vx>>1] - 128;
            a2 = psrcU[vx>>1] - 128;

            a0 = (a3 * param[0]) >> 20;
            a1 = (a3 * param[1] + a2 * param[3]) >> 20;
            a2 = (a2 * param[2]) >> 20;

            a3 = psrcY[vx + 1];

            *(outbuf)   = (ccClip255[(a4 + a2)]);
            *(outbuf + 1) = (ccClip255[(a4 - a1)]);
            *(outbuf + 2) = (ccClip255[(a4 + a0)]);

            a4 = psrcY[vx + in_stride];

            *(outbuf + 3) = (ccClip255[(a3 + a2)]);
            *(outbuf + 4) = (ccClip255[(a3 - a1)]);
            *(outbuf + 5) = (ccClip255[(a3 + a0)]);

            a3 = psrcY[vx + in_stride + 1];

            outbuf += step_y;

            *(outbuf)   = (ccClip255[(a4 + a2)]);
            *(outbuf + 1) = (ccClip255[(a4 - a1)]);
            *(outbuf + 2) = (ccClip255[(a4 + a0)]);

            *(outbuf + 3) = (ccClip255[(a3 + a2)]);
            *(outbuf + 4) = (ccClip255[(a3 - a1)]);
            *(outbuf + 5) = (ccClip255[(a3 + a0)]);
            outbuf -= step_y;

            vx += 2;
            outbuf += 6;
        }
        start_out_buf += (step_y << 1);
    }

    return VO_ERR_NONE;

}

static VOCCRETURNCODE YUVPlanarToRGB24_noresize_norotation(ClrConvData *conv_data, const VO_U32 is420Planar,
        VO_S32 *x_resize_tab, VO_S32 *y_resize_tab)
{
    VO_S32 vx = 0, width, height;
    VO_U8 *psrc_y = conv_data->pInBuf[0];
    VO_U8 *psrc_u = conv_data->pInBuf[1];
    VO_U8 *psrc_v = conv_data->pInBuf[2];
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_U8	*start_out_buf, *out_buf;
    VO_S32 skip = 0;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 out_width = conv_data->nOutWidth;
    VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    mb_skip = conv_data->mb_skip;

    y = psrc_y;
    u = psrc_u;
    v = psrc_v;
    //add end

    start_out_buf = out_buf = conv_data->pOutBuf[0];

    do
    {
        vx = out_width;
        out_buf = start_out_buf;
        do
        {
            if(mb_skip) skip = *(mb_skip++);

            if(!skip)
            {
                width = vx < 16 ? vx : 16;
                height = out_height < 16 ? out_height : 16;
                yuv420_rgb24_mb[(width==16)&&(height==16)](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width, height, uin_stride, vin_stride);
            }

            psrc_y += 16;
            psrc_u += 8;
            psrc_v += 8;
            out_buf += 48;
        }
        while((vx -= 16) > 0);

        psrc_y = y = y + (in_stride << 4);
        psrc_u = u = u + (uin_stride << 3);
        psrc_v = v = v + (vin_stride << 3);
        start_out_buf += (out_stride << 4);
    }
    while((out_height -= 16) > 0);

    return VO_ERR_NONE;

}

VOCCRETURNCODE YUVPlanarToRGB24_noresize(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    if(conv_data->nRotationType == FLIP_Y)
    {
        YUVPlanarToRGB24_noresize_FLIPY(conv_data, is420Planar, x_resize_tab, y_resize_tab);
    }
    else if (ROTATION_90L == conv_data->nRotationType)
    {
        YUVPlanarToRGB24_noresize_rotation90L(conv_data, is420Planar, x_resize_tab, y_resize_tab);
    }
    else if(ROTATION_90R == conv_data->nRotationType)
    {
        YUVPlanarToRGB24_noresize_rotation90R(conv_data, is420Planar, x_resize_tab, y_resize_tab);
    }
    else
    {
        YUVPlanarToRGB24_noresize_norotation(conv_data, is420Planar, x_resize_tab, y_resize_tab);
    }

    return VO_ERR_NONE;
}
#endif //0

//static VOCCRETURNCODE YUVPlanarToRGB24_noresize_FLIPY(ClrConvData *conv_data, const VO_U32 is420Planar,
//													  VO_S32 *x_resize_tab, VO_S32 *y_resize_tab,struct CC_HND * cc_hnd)
//{
//	VO_U32 vx = 0, vy;
//	const VO_U32 in_stride = conv_data->nInStride , out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
//	const VO_S32 *param = cc_hnd->yuv2rgbmatrix;
//	VO_U8 *outbuf, *start_out_buf;
//	VO_U8 *psrcY, *psrcU, *psrcV;
//
//	VO_S32 out_stride = conv_data->nOutStride;
//	VO_S32 step_y;
//
//	step_y = -out_stride;
//	start_out_buf = conv_data->pOutBuf[0]+((out_height-1)*out_stride);
//
//
//	for(vy = 0; vy < out_height; vy+=2) {
//		outbuf = start_out_buf;
//
//		vx =  vy*in_stride;
//		psrcY = conv_data->pInBuf[0] + vx;
//
//		psrcU = conv_data->pInBuf[1] + (vx>>(2-(!is420Planar)));
//		psrcV = conv_data->pInBuf[2] + (vx>>(2-(!is420Planar)));
//
//		for(vx = 0; vx < out_width;) {
//			VO_S32 a0, a1, a2, a3, a4;
//			a4 = psrcY[vx];
//			a3 = psrcV[vx>>1] - 128;
//			a2 = psrcU[vx>>1] - 128;
//
//			a0 = (a3 * param[0]) >> 20;
//			a1 = (a3 * param[1] + a2 *param[3]) >> 20;
//			a2 = (a2 * param[2]) >> 20;
//
//			a3 = psrcY[vx + 1];
//
//			*(outbuf)   = (ccClip255[(a4 + a2)]);
//			*(outbuf+1) = (ccClip255[(a4 - a1)]);
//			*(outbuf+2) = (ccClip255[(a4 + a0)]);
//
//			a4 = psrcY[vx + in_stride];
//
//			*(outbuf+3) = (ccClip255[(a3 + a2)]);
//			*(outbuf+4) = (ccClip255[(a3 - a1)]);
//			*(outbuf+5) = (ccClip255[(a3 + a0)]);
//
//			a3 = psrcY[vx + in_stride + 1];
//
//			outbuf += step_y;
//
//			*(outbuf)   = (ccClip255[(a4 + a2)]);
//			*(outbuf+1) = (ccClip255[(a4 - a1)]);
//			*(outbuf+2) = (ccClip255[(a4 + a0)]);
//
//			*(outbuf+3) = (ccClip255[(a3 + a2)]);
//			*(outbuf+4) = (ccClip255[(a3 - a1)]);
//			*(outbuf+5) = (ccClip255[(a3 + a0)]);
//			outbuf -= step_y;
//
//			vx += 2;
//			outbuf += 6;
//		}
//		start_out_buf += (step_y<<1);
//	}
//
//	return VO_ERR_NONE;
//
//}

//static VOCCRETURNCODE YUVPlanarToRGB24_noresize_norotation(ClrConvData *conv_data, const VO_U32 is420Planar, VO_S32 *x_resize_tab, VO_S32 *y_resize_tab)
//{
//	VO_S32 vx = 0, width, height;
//	VO_U8 *psrc_y = conv_data->pInBuf[0];
//	VO_U8 *psrc_u = conv_data->pInBuf[1];
//	VO_U8 *psrc_v = conv_data->pInBuf[2];
//	VO_U8 *y, *u, *v;
//	VO_U8 *mb_skip;
//	VO_U8	*start_out_buf, *out_buf;
//	VO_S32 skip = 0;
//	VO_S32 in_stride, uin_stride,vin_stride;
//	VO_S32 out_width = conv_data->nOutWidth;
//	VO_S32 out_height = conv_data->nOutHeight;
//	const VO_S32 out_stride = conv_data->nOutStride;
//
//	in_stride  = conv_data->nInStride;
//	uin_stride = conv_data->nInUVStride << (!is420Planar);
//	vin_stride = conv_data->nInUVStride << (!is420Planar);
//
//	mb_skip = conv_data->mb_skip;
//
//	y = psrc_y;
//	u = psrc_u;
//	v = psrc_v;
//	//add end
//
//	start_out_buf = out_buf = conv_data->pOutBuf[0];
//
//	do{
//		vx = out_width;
//		out_buf = start_out_buf;
//		do{
//			if(mb_skip) skip = *(mb_skip++);
//
//			if(!skip){
//				width = vx < 16 ? vx : 16;
//				height = out_height < 16 ? out_height : 16;
//				yuv420_rgb24_mb[(width==16)&&(height==16)](psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, width, height, uin_stride, vin_stride);
//			}
//
//			psrc_y += 16;
//			psrc_u += 8;
//			psrc_v += 8;
//			out_buf += 48;
//		}while((vx -= 16)> 0);
//
//		psrc_y = y = y + (in_stride << 4);
//		psrc_u = u = u + (uin_stride << 3);
//		psrc_v = v = v + (vin_stride << 3);
//		start_out_buf += (out_stride << 4);
//	}while((out_height -= 16)>0);
//
//	return VO_ERR_NONE;
//
//}
//
//VOCCRETURNCODE YUVPlanarToRGB24_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
//{
//	const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);
//
//	if (ROTATION_DISABLE == conv_data->nRotationType){
//		YUVPlanarToRGB24_noresize_norotation(conv_data, is420Planar, x_resize_tab, y_resize_tab);
//	}else if (FLIP_Y == conv_data->nRotationType){
//		YUVPlanarToRGB24_noresize_FLIPY(conv_data, is420Planar, x_resize_tab, y_resize_tab,cc_hnd);
//	}else{
//		YUVPlanarToRGB24_noresize_rotation(conv_data, x_resize_tab,y_resize_tab,cc_hnd);
//	}
//}

VOCCRETURNCODE YUVPlanarToRGB24_noresize_rotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U32 vx = 0, vy = 0;
    const VO_U32 in_stride = conv_data->nInStride, out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;
    VO_U8 *psrc_y, *psrc_u, *psrc_v, *out_buf, *start_out_buf = NULL;
    VO_U8 *mb_skip;
    VO_S32 skip = 0;
    //VO_S32 *param = cc_hnd->yuv2rgbmatrix;
    VO_S32 step_x = 0, step_y = 0;
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    VO_U32 uin_stride = conv_data->nInUVStride << (!is420Planar);
    VO_U32 vin_stride = conv_data->nInUVStride << (!is420Planar);

    mb_skip = conv_data->mb_skip;

    if((out_width < 16) || (out_height < 16))
        goto Normal_convert;

    if(conv_data->nRotationType == ROTATION_90L)
    {
        YUV420PLANARTORGB24_NEWMB = YUV420PlanarToRGB24_NEWMB_rotation_90L;
        start_out_buf = conv_data->pOutBuf[0] + (out_width - 1) * out_stride;
        step_x = -(out_stride << 4);
        step_y = 48;
    }
    else if(conv_data->nRotationType == ROTATION_90R)
    {
        YUV420PLANARTORGB24_NEWMB = YUV420PlanarToRGB24_NEWMB_rotation_90R;
        start_out_buf = conv_data->pOutBuf[0] + (out_height << 1) - 4;
        step_x = (out_stride << 4);
        step_y = -48;
    }
    else if(conv_data->nRotationType == ROTATION_180)
    {
        YUV420PLANARTORGB24_NEWMB = YUV420PlanarToRGB24_NEWMB_rotation_180;
        //start_out_buf = conv_data->pOutBuf[0] + (out_height* out_stride) - 4;
        start_out_buf = conv_data->pOutBuf[0] + (out_height - 15) * out_stride - 32;
        step_x = -48;
        step_y = -(out_stride << 4);
    }

    /*prepare resize*/
    for(vy = 16; vy <= out_height; vy += 16)
    {
        VO_S32 offset = (vy - 16) * in_stride;

        psrc_y = conv_data->pInBuf[0] + offset;
        offset = ((vy - 16) >> 1) * uin_stride;
        psrc_u = conv_data->pInBuf[1] + offset;
        psrc_v = conv_data->pInBuf[2] + offset;

        out_buf = start_out_buf;
        for(vx = 16; vx <= out_width; vx += 16)
        {
            if(mb_skip) skip = *(mb_skip++);

            if(!skip)
            {
                YUV420PLANARTORGB24_NEWMB(psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, 16, 16, uin_stride, vin_stride);
            }

            psrc_y += 16;
            psrc_u += 8;
            psrc_v += 8;
            out_buf += step_x;

        }
        start_out_buf += step_y;
    }

    vx = vx - 16;
    vy = vy - 16;


Normal_convert:

    if(vx != out_width) /*not multiple of 16*/
    {

        ClrConvData tmp_conv_data;
        tmp_conv_data = *conv_data;

        tmp_conv_data.nOutWidth = out_width - vx;
        tmp_conv_data.nOutHeight = vy;

        tmp_conv_data.pInBuf[0] = conv_data->pInBuf[0] + vx;
        tmp_conv_data.pInBuf[1] = conv_data->pInBuf[1] + (vx >> 1);
        tmp_conv_data.pInBuf[2] = conv_data->pInBuf[2] + (vx >> 1);
        if(conv_data->nRotationType == ROTATION_90L)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0];
        }
        else if(conv_data->nRotationType == ROTATION_90R)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + out_stride * (vx + 1) - (vy << 1) ;
        }
        else if(conv_data->nRotationType == ROTATION_180)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + ((out_width - vx) << 1);
        }
        else
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (vx << 1);
        }

        NewYUVPlanarToRGB24_normal(&tmp_conv_data, 0, is420Planar, cc_hnd);
    }

    if(vy != out_height)
    {
        ClrConvData tmp_conv_data;
        tmp_conv_data = *conv_data;

        tmp_conv_data.pInBuf[0] = conv_data->pInBuf[0] + vy * in_stride;
        tmp_conv_data.pInBuf[1] = conv_data->pInBuf[1] + (vy >> is420Planar) * (in_stride >> 1);
        tmp_conv_data.pInBuf[2] = conv_data->pInBuf[2] + (vy >> is420Planar) * (in_stride >> 1);

        if(conv_data->nRotationType == ROTATION_90L)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (vy << 1);
        }
        else if(conv_data->nRotationType == ROTATION_90R)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0];
        }
        else if(conv_data->nRotationType == ROTATION_180)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (out_height - vy) * out_stride;
        }
        else
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + vy * out_stride;
        }

        tmp_conv_data.nOutWidth = out_width;
        tmp_conv_data.nOutHeight = out_height - vy;
        NewYUVPlanarToRGB24_normal(&tmp_conv_data, 0, is420Planar, cc_hnd);
    }

    return VO_ERR_NONE;
}


VOCCRETURNCODE YUV422InterlaceToRGB16_noresize(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U32 vx = 0, vy;
    VO_U32 in_stride = conv_data->nInStride , out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;
    const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    VO_S32 YUV422Id = 0;
    VO_U8 *psrc, *out_buf, *start_out_buf;
    VO_S32 *param = cc_hnd->yuv2rgbmatrix;
    VO_S32 step_x, step_y;

    if (conv_data->nInType == VO_COLOR_YUYV422_PACKED || conv_data->nInType == VO_COLOR_YUYV422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_YUYV422_PACKED ? 0 : 4;
        YUV422Id = IDYUYV422 + Y_switch;
    }
    else if (conv_data->nInType == VO_COLOR_YVYU422_PACKED || conv_data->nInType == VO_COLOR_YVYU422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_YVYU422_PACKED ? 0 : 4;
        YUV422Id = IDYVYU422 + Y_switch;
    }
    else if (conv_data->nInType == VO_COLOR_UYVY422_PACKED || conv_data->nInType == VO_COLOR_UYVY422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_UYVY422_PACKED ? 0 : 4;
        YUV422Id = IDUYVY422 + Y_switch;
    }
    else if (conv_data->nInType == VO_COLOR_VYUY422_PACKED || conv_data->nInType == VO_COLOR_VYUY422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_VYUY422_PACKED ? 0 : 4;
        YUV422Id = IDVYUY422 + Y_switch;
    }


    if(conv_data->nRotationType == ROTATION_90L)
    {
        YUV422INTERLACETORGB16_MB = Yuv422Interlace_mb[YUV422Id][1];
        start_out_buf = conv_data->pOutBuf[0] + (out_width - 1) * out_stride;
        step_x = -(out_stride << 4);
        step_y = 32;
    }
    else if(conv_data->nRotationType == ROTATION_90R)
    {
        YUV422INTERLACETORGB16_MB = Yuv422Interlace_mb[YUV422Id][2];
        start_out_buf = conv_data->pOutBuf[0] + (out_height << 1) - 4;
        step_x = (out_stride << 4);
        step_y = -32;
    }
    else if(conv_data->nRotationType == ROTATION_180)
    {
        YUV422INTERLACETORGB16_MB = Yuv422Interlace_mb[YUV422Id][3];
        start_out_buf = conv_data->pOutBuf[0] + (out_height * out_stride) - 4;
        step_x = -32;
        step_y = -(out_stride << 4);
    }
    else
    {
        YUV422INTERLACETORGB16_MB = Yuv422Interlace_mb[YUV422Id][0];
        start_out_buf = conv_data->pOutBuf[0];
        step_x = 32;
        step_y = (out_stride << 4);
    }

    /*prepare resize*/
    for(vy = 0; vy < out_height; vy += 16)
    {
        VO_S32 offset = vy * in_stride;

        psrc = conv_data->pInBuf[0] + offset;
        out_buf = start_out_buf;

        for(vx = 0; vx < out_width; vx += 16)
        {
            YUV422INTERLACETORGB16_MB(psrc, in_stride, out_buf, out_stride, param, isRGB565);
            psrc += 32;
            out_buf += step_x;
        }
        start_out_buf += step_y;
    }
    return VO_ERR_NONE;
}

VOCCRETURNCODE YUV422InterlaceToRGB24_noresize(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U32 vx = 0, vy;//, rx = 0xFFFFFFFF, ry = 0xFFFFFFFF;
    const VO_U32 in_stride = conv_data->nInStride, out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    VO_U8 *outbuf, *start_outbuf = NULL;
    VO_S32 *param = cc_hnd->yuv2rgbmatrix;
    VO_S32 YUV422Id = 0;
    VO_U8 *psrc;
    VO_S32 step_y = 0, step_x = 0, out_stride = conv_data->nOutStride;
    VO_U32 y_shift, u_shift, v_shift ;
    VO_U32 y0 = 0, y1 = 2;

    if (conv_data->nInType == VO_COLOR_YUYV422_PACKED || conv_data->nInType == VO_COLOR_YUYV422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_YUYV422_PACKED ? 0 : 4;
        YUV422Id = IDYUYV422 + Y_switch;
    }
    else if (conv_data->nInType == VO_COLOR_YVYU422_PACKED || conv_data->nInType == VO_COLOR_YVYU422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_YVYU422_PACKED ? 0 : 4;
        YUV422Id =  IDYVYU422 + Y_switch;
    }
    else if (conv_data->nInType == VO_COLOR_UYVY422_PACKED || conv_data->nInType == VO_COLOR_UYVY422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_UYVY422_PACKED ? 0 : 4;
        YUV422Id =  IDUYVY422 + Y_switch;
    }
    else if (conv_data->nInType == VO_COLOR_VYUY422_PACKED || conv_data->nInType == VO_COLOR_VYUY422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_VYUY422_PACKED ? 0 : 4;
        YUV422Id =  IDVYUY422 + Y_switch;
    }

    if(YUV422Id >= 4)
    {
        YUV422Id -= 4;
        y0 = 2;
        y1 = 0;
    }
    y_shift = (YUV422Id > 1);
    u_shift = u_shift_tab[YUV422Id];
    v_shift = v_shift_tab[YUV422Id];

    /*start of output address*/
    if(conv_data->nRotationType == ROTATION_DISABLE) /*disable rotation*/
    {
        step_y = 3;
        step_x = out_stride;
        start_outbuf = conv_data->pOutBuf[0];
    }
    else if(conv_data->nRotationType == ROTATION_90L)  /*rotation90L*/
    {
        step_y = -out_stride;
        step_x = 3;
        start_outbuf = conv_data->pOutBuf[0] - (out_width - 1) * step_y;
    }
    else if(conv_data->nRotationType == ROTATION_90R)  /*rotation90R*/
    {
        step_y = out_stride;
        step_x = -3;
        start_outbuf = conv_data->pOutBuf[0] + (out_height * 3) - 3;
    }

    for(vy = 0; vy < out_height; vy++)
    {
        outbuf = start_outbuf;

        psrc = conv_data->pInBuf[0] + vy * in_stride;

        for(vx = 0; vx < out_width; vx += 2)
        {
            VO_S32 a0, a1, a2, a3;

            a3 = psrc[v_shift] - 128;
            a2 = psrc[u_shift] - 128;

            a0 = (a3 * param[0]) >> 20;
            a1 = (a3 * param[1] + a2 * param[3]) >> 20;
            a2 = (a2 * param[2]) >> 20;

            a3 = psrc[y_shift + y0];

            *(outbuf)   = (ccClip255[(a3 + a2)]);
            *(outbuf + 1) = (ccClip255[(a3 - a1)]);
            *(outbuf + 2) = (ccClip255[(a3 + a0)]);

            outbuf += step_y;

            a3 = psrc[y_shift + y1];

            *(outbuf) = (ccClip255[(a3 + a2)]);
            *(outbuf + 1) = (ccClip255[(a3 - a1)]);
            *(outbuf + 2) = (ccClip255[(a3 + a0)]);
            outbuf += step_y;

            psrc += 4;
        }
        start_outbuf += step_x;
    }
    return VO_ERR_NONE;
}

VOCCRETURNCODE YUV422PlanarToYUV420Planar(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_S32 i;
    VO_S32 in_stride  = conv_data->nInStride;
    VO_S32 out_width  = conv_data->nOutWidth;
    VO_S32 out_height = conv_data->nOutHeight;
    VO_S32 out_stride = conv_data->nOutStride;

    for(i = 0; i < out_height; i++)
    {
        memcpy(conv_data->pOutBuf[0] + i * out_stride, conv_data->pInBuf[0] + i * in_stride, out_width);
    }

    in_stride  = (conv_data->nInStride >> 1);
    out_width  = (conv_data->nOutWidth >> 1);
    out_height = (conv_data->nOutHeight >> 1);
    out_stride = (conv_data->nOutStride >> 1);

    for(i = 0; i < out_height; i++)
    {
        memcpy(conv_data->pOutBuf[1] + i * out_stride, conv_data->pInBuf[1] + 2 * i * in_stride, out_width);
        memcpy(conv_data->pOutBuf[2] + i * out_stride, conv_data->pInBuf[2] + 2 * i * in_stride, out_width);
    }
    return VO_ERR_NONE;
}
#endif //COLCONVENABLE

// Resize
VOCCRETURNCODE YUVPlanarToRGB16_resize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
#ifndef VOARMV7
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
#else
    VO_S32 block_width  = (conv_data->nOutWidth + 7) / 8;
    VO_S32 block_height = (conv_data->nOutHeight) / 8;
    VO_S32 height_remainder = 0;
#endif
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

#ifdef VOARMV7
    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //	VO_U8 *pYUVBuffer = NULL;
#endif


    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

#if DEBUG_DLL
    do
    {
        VO_S8 buff[128];
        sprintf(buff, "%s block_width %d block_height %d\n", __FUNCTION__, mb_width, mb_width);
        DEBUG(buff);
    }
    while(0);
#endif

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

#ifdef VOARMV7
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = out_buf = conv_data->pOutBuf[0];

#ifdef VOARMV7
    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];

        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x8_s_armv7(width, height, y, u, v,
                                       dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb16_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            dst += 2 * width;
            x_scale_tab += (width * 3 / 2 + 1);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start += (height * out_stride);
    }

    height_remainder = conv_data->nOutHeight - block_height * 8;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x2n_s_armv7(width, height, y, u, v,
                                        dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb16_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            dst += 2 * width;
            x_scale_tab += (width * 3 / 2 + 1);
        }
    }
#else
    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);

            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_s(width, height, y, u, v,
                               dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width << 1);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
#endif

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB16_resize_noRotation_half(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 skip = 0;

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

#if DEBUG_DLL
    do
    {
        VO_S8 buff[128];
        sprintf(buff, "%s block_width %d block_height %d\n", __FUNCTION__, mb_width, mb_width);
        DEBUG(buff);
    }
    while(0);
#endif

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;

    dst_start = out_buf = conv_data->pOutBuf[0];

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];

            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_s_c_half(width, height, y, u, v,
                                      dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width << 1);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB16_resize_noRotation_3quarter(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 skip = 0;
    VO_S32 i, j, width, height;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = out_buf = conv_data->pOutBuf[0];

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];

            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_s_c_3quarter(width, height, y, u, v,
                                          dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width << 1);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB16_resize_noRotation_1point5(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 skip = 0;
    VO_S32 i, j, width, height;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = out_buf = conv_data->pOutBuf[0];

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];

            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_s_c_1point5(width, height, y, u, v,
                                         dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width << 1);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB16_resize_noRotation_double(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 skip = 0;
    VO_S32 i, j, width, height;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = out_buf = conv_data->pOutBuf[0];

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];

            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_s_c_double(width, height, y, u, v,
                                        dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width << 1);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB16_resize_424X320_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_S32 i, j;//, width, height;
    VO_S32 in_stride, uin_stride, vin_stride;
    const VO_S32 out_stride = conv_data->nOutStride;
    VO_S32 a0, a1, a2, a3, a4, a5, a7, a8;// a6,
    VO_S32 b0, b1;
    VO_S32 c0, c1, c2, c3;//, c4;//,c5;
    //VO_S32 aa, bb;


    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride;
    vin_stride = conv_data->nInUVStride;
    dst_start = out_buf = conv_data->pOutBuf[0];

    for(j = 0; j < 80; j++)
    {
        VO_U8 *src_y0 = y + 3 * j * in_stride; // TBD
        VO_U8 *src_u0 = u + (((3 * j) >> 1) * uin_stride);
        VO_U8 *src_v0 = v + (((3 * j) >> 1) * vin_stride);
        VO_U8 *src_y1 = y + 3 * j * in_stride;
        VO_U8 *src_y2 = y + (3 * j + 1) * in_stride;
        VO_U8 *src_u1 = u + (((3 * j + 1) >> 1) * uin_stride);
        VO_U8 *src_v1 = v + (((3 * j + 1) >> 1) * vin_stride);
        VO_U8 *src_y3 = y + (3 * j + 2) * in_stride;

        dst = dst_start + j * out_stride * 4;

        for(i = 0; i < 106; i++)
        {
            // u, v
            b0 = 3 * i;
            b1 = (6 * i + 1) >> 2;

            a2 = src_u0[b1];
            a3 = src_v0[b1];

            a4 = c0 = src_y0[b0];
            c2 = src_y1[b0 + in_stride];
            a5 = (c0 + c2 * 3) >> 2;

            a2 -= 128;
            a3 -= 128;

            a0 = (a3 * ConstV1);
            a1 = (a3 * ConstV2 + a2 * ConstU2);
            a2 = (a2 * ConstU1);

            a4 = (a4 - 16) * ConstY;
            a5 = (a5 - 16) * ConstY;

            a4 = ((ccClip31[(a4 + a0)>>23]) << 11) | ((ccClip63[(a4 - a1)>>22]) << 5) | (ccClip31[(a4 + a2)>>23]);
            a7 = ((ccClip31[(a5 + a0)>>23]) << 11) | ((ccClip63[(a5 - a1)>>22]) << 5) | (ccClip31[(a5 + a2)>>23]);


            c1 = src_y0[b0 + 1];
            a5 = ((c0 + c1 * 3)) >> 2;
            a8 = ((c0 + c1 * 3) + (c2 + src_y1[b0 + in_stride + 1] * 3) * 3) >> 4;

            a5 = (a5 - 16) * ConstY;
            a8 = (a8 - 16) * ConstY;

            a5 = ((ccClip31[(a5 + a0)>>23]) << 11) | ((ccClip63[(a5 - a1)>>22]) << 5) | (ccClip31[(a5 + a2)>>23]); //|a4;
            a8 = ((ccClip31[(a8 + a0)>>23]) << 11) | ((ccClip63[(a8 - a1)>>22]) << 5) | (ccClip31[(a8 + a2)>>23]); //|a7;

            a4 |= (a5 << 16);
            a7 |= (a8 << 16);
            *((VO_U32 *)dst) = a4;
            *((VO_U32 *)(dst + out_stride)) = a7;

            a2 = src_u1[b1];
            a3 = src_v1[b1];

            c0 = src_y2[b0];
            c1 = src_y2[b0+in_stride];
            c2 = src_y3[b0];
            c3 = src_y3[b0+in_stride];

            a4 = (c0 + c1) >> 1;
            a5 = (c2 * 3 + c3) >> 2;

            a2 -= 128;
            a3 -= 128;

            a0 = (a3 * ConstV1);
            a1 = (a3 * ConstV2 + a2 * ConstU2);
            a2 = (a2 * ConstU1);

            a4 = (a4 - 16) * ConstY;
            a5 = (a5 - 16) * ConstY;

            a4 = ((ccClip31[(a4 + a0)>>23]) << 11) | ((ccClip63[(a4 - a1)>>22]) << 5) | (ccClip31[(a4 + a2)>>23]);
            a7 = ((ccClip31[(a5 + a0)>>23]) << 11) | ((ccClip63[(a5 - a1)>>22]) << 5) | (ccClip31[(a5 + a2)>>23]);

            a5 = ((c0 + src_y2[b0+1] * 3) + (c1 + src_y2[b0+in_stride+1] * 3)) >> 3;
            a8 = ((c2 + src_y3[b0+1] * 3) * 3 + (c3 + src_y3[b0+in_stride+1] * 3)) >> 4;

            a5 = (a5 - 16) * ConstY;
            a8 = (a8 - 16) * ConstY;

            a5 = ((ccClip31[(a5 + a0)>>23]) << 11) | ((ccClip63[(a5 - a1)>>22]) << 5) | (ccClip31[(a5 + a2)>>23]); //|a4;
            a8 = ((ccClip31[(a8 + a0)>>23]) << 11) | ((ccClip63[(a8 - a1)>>22]) << 5) | (ccClip31[(a8 + a2)>>23]); //|a7;

            a4 |= (a5 << 16);
            a7 |= (a8 << 16);
            *((VO_U32 *)(dst + 2 * out_stride)) = a4;
            *((VO_U32 *)(dst + 3 * out_stride)) = a7;

            dst += 4;

            // u, v
            b0 = 3 * i + 1;
            b1 = (6 * i + 4) >> 2;

            a2 = src_u0[b1];
            a3 = src_v0[b1];

            c0 = src_y0[b0];
            c1 = src_y0[b0+1];
            a4 = ((c0 + c1)) >> 1;
            a5 = ((c0 + c1) + (src_y1[b0+in_stride] + src_y1[b0+in_stride+1]) * 3) >> 3;

            a2 -= 128;
            a3 -= 128;

            a0 = (a3 * ConstV1);
            a1 = (a3 * ConstV2 + a2 * ConstU2);
            a2 = (a2 * ConstU1);

            a4 = (a4 - 16) * ConstY;
            a5 = (a5 - 16) * ConstY;

            a4 = ((ccClip31[(a4 + a0)>>23]) << 11) | ((ccClip63[(a4 - a1)>>22]) << 5) | (ccClip31[(a4 + a2)>>23]);
            a7 = ((ccClip31[(a5 + a0)>>23]) << 11) | ((ccClip63[(a5 - a1)>>22]) << 5) | (ccClip31[(a5 + a2)>>23]);

            a5 = ((c1 * 3 + src_y0[b0+2])) >> 2;
            a8 = ((c1 * 3 + src_y1[b0+2]) + (src_y1[b0+in_stride+1] * 3 + src_y1[b0+in_stride+2]) * 3) >> 4;

            a5 = (a5 - 16) * ConstY;
            a8 = (a8 - 16) * ConstY;

            a5 = ((ccClip31[(a5 + a0)>>23]) << 11) | ((ccClip63[(a5 - a1)>>22]) << 5) | (ccClip31[(a5 + a2)>>23]); //|a4;
            a8 = ((ccClip31[(a8 + a0)>>23]) << 11) | ((ccClip63[(a8 - a1)>>22]) << 5) | (ccClip31[(a8 + a2)>>23]); //|a7;

            a4 |= (a5 << 16);
            a7 |= (a8 << 16);
            *((VO_U32 *)dst) = a4;
            *((VO_U32 *)(dst + out_stride)) = a7;

            a2 = src_u1[b1];
            a3 = src_v1[b1];

            a4 = (src_y2[b0] + src_y2[b0+1] + src_y2[b0+in_stride] + src_y2[b0+in_stride+1]) >> 2;
            a5 = ((src_y3[b0] + src_y3[b0+1]) * 3 + (src_y3[b0+in_stride] + src_y3[b0+in_stride+1])) >> 3;

            a2 -= 128;
            a3 -= 128;

            a0 = (a3 * ConstV1);
            a1 = (a3 * ConstV2 + a2 * ConstU2);
            a2 = (a2 * ConstU1);

            a4 = (a4 - 16) * ConstY;
            a5 = (a5 - 16) * ConstY;

            a4 = ((ccClip31[(a4 + a0)>>23]) << 11) | ((ccClip63[(a4 - a1)>>22]) << 5) | (ccClip31[(a4 + a2)>>23]);
            a7 = ((ccClip31[(a5 + a0)>>23]) << 11) | ((ccClip63[(a5 - a1)>>22]) << 5) | (ccClip31[(a5 + a2)>>23]);

            a5 = ((src_y2[b0+1] * 3 + src_y2[b0+2]) + (src_y2[b0+in_stride+1] * 3 + src_y2[b0+in_stride+2])) >> 3;
            a8 = ((src_y3[b0+1] * 3 + src_y3[b0+2]) * 3 + (src_y3[b0+in_stride+1] * 3 + src_y3[b0+in_stride+2])) >> 4;

            a5 = (a5 - 16) * ConstY;
            a8 = (a8 - 16) * ConstY;

            a5 = ((ccClip31[(a5 + a0)>>23]) << 11) | ((ccClip63[(a5 - a1)>>22]) << 5) | (ccClip31[(a5 + a2)>>23]); //|a4;
            a8 = ((ccClip31[(a8 + a0)>>23]) << 11) | ((ccClip63[(a8 - a1)>>22]) << 5) | (ccClip31[(a8 + a2)>>23]); //|a7;

            a4 |= (a5 << 16);
            a7 |= (a8 << 16);
            *((VO_U32 *)(dst + 2 * out_stride)) = a4;
            *((VO_U32 *)(dst + 3 * out_stride)) = a7;

            dst += 4;

        }
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB32_resize_norotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
#if !defined(VOARMV7) && !defined(VOX86)
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
#elif defined(VOARMV7)
    VO_S32 block_width  = (conv_data->nOutWidth + 7) / 8;
    VO_S32 block_height = (conv_data->nOutHeight) / 8;
    VO_S32 height_remainder = 0;
#else
    VO_S32 block_width	= (conv_data->nOutWidth) / 16;
    VO_S32 block_height = (conv_data->nOutHeight) / 16;
    VO_S32 height_remainder = 0;
    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
#endif
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
#ifdef VOARMV7
    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
#endif

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

#if DEBUG_DLL
    do
    {
        FILE *f;
        VO_S8 buff[128];
        f = fopen("D:/cc.log", "a+");
        sprintf(buff, "%s block_width %d block_height %d\n", __FUNCTION__, block_width, block_width);
        fputs(buff, f);
        fclose(f);
    }
    while(0);
#endif


#ifdef VOARMV7
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

#ifdef VOX86
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 16 * 16;
    dst_startv = cc_hnd->pYUVBuffer + 16 * 16 + 16 * 16 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif


    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = out_buf = conv_data->pOutBuf[0];

#if defined(VOARMV7)
    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];

        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x8_s_armv7(width, height, y, u, v,
                                       dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 / 2 + 1);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start += (height * out_stride);
    }

    height_remainder = conv_data->nOutHeight - block_height * 8;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x2n_s_armv7(width, height, y, u, v,
                                        dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 / 2 + 1);
        }
    }
#elif defined(VOX86)
    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];

        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start += (height * out_stride);
    }

    height_remainder = conv_data->nOutHeight - block_height * 16;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
    }
#else
    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {

            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);

            if((!skip) && (width != 0) && (height != 0))
            {
                cc_yuv420_rgb32_mb_s(width, height, y, u, v,
                                     dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            }
            x_scale_tab += (width * 3 + 1);
            dst += (width << 2);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
#endif

    return VO_ERR_NONE;
}



#ifdef VOX86
VOCCRETURNCODE BlkLineClrConv(ClrConvEnv *ClrConvEnvP, VO_S32 thdIdx, VO_S32 blk_height)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
#ifdef VOX86
    VO_U8 *psrc_y, *psrc_u, *psrc_v;
    ClrConvData *conv_data = ClrConvEnvP->conv_data;
    VO_S32 *x_resize_tab = ClrConvEnvP->x_resize_tab;
    VO_S32 *y_resize_tab = ClrConvEnvP->y_resize_tab;
    struct CC_HND *cc_hnd = ClrConvEnvP->cc_hnd;
#endif

    VO_S32 block_width	= (conv_data->nOutWidth) / 16;
    VO_S32 block_height = (conv_data->nOutHeight) / 16;
    VO_S32 height_remainder = 0;
    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);


#if DEBUG_DLL
    do
    {
        FILE *f;
        VO_S8 buff[128];
        f = fopen("D:/cc.log", "a+");
        sprintf(buff, "%s block_width %d block_height %d\n", __FUNCTION__, block_width, block_width);
        fputs(buff, f);
        fclose(f);
    }
    while(0);
#endif


#ifdef VOX86
    dst_starty = cc_hnd->pYUVBuffer + ClrConvEnvP->thdBufSize * thdIdx;
    dst_startu = dst_starty + 16 * 16;
    dst_startv = dst_startu + 16 * 16 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif


#ifdef VOX86
    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab +  (16 * 3 + 1) * blk_height ;
#else
    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
#endif
#ifdef VOX86
    dst_start = out_buf = conv_data->pOutBuf[0] + out_stride * blk_height * 16;
#else
    dst_start = out_buf = conv_data->pOutBuf[0];
#endif

#if defined(VOARMV7)
    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];

        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x8_s_armv7(width, height, y, u, v,
                                       dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 / 2 + 1);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start += (height * out_stride);
    }

    height_remainder = conv_data->nOutHeight - block_height * 8;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x2n_s_armv7(width, height, y, u, v,
                                        dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 / 2 + 1);
        }
    }
#elif defined(VOX86)
    switch(cc_hnd->ClrConvFunIndex)
    {
    case RGB32ResizeNoRot:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;
    case RGB32ResizeNoRotHalf:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86_half(width, height, y, u, v,
                                            dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;

    case RGB32ResizeRot90l:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;
    case RGB32ResizeRot90lHalf:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86_half(width, height, y, u, v,
                                            dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;
    case RGB32ResizeRot90r:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;
    case RGB32ResizeRot90rHalf:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86_half(width, height, y, u, v,
                                            dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;
    case RGB32ResizeRot180:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;
    case ARGB32ResizeNoRot:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420toargb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;

    case ARGB32ResizeNoRotHalf:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86_half(width, height, y, u, v,
                                            dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;
    case ARGB32ResizeRot90l:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;
    case ARGB32ResizeRot90lHalf:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86_half(width, height, y, u, v,
                                            dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;
    case ARGB32ResizeRot90r:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;
    case ARGB32ResizeRot90rHalf:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86_half(width, height, y, u, v,
                                            dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;
    case ARGB32ResizeRot180:
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
        break;
    case RGB32NoResizeNoRot:
        if((blk_height + 1) * 16 > conv_data->nOutHeight)
            height = conv_data->nOutHeight - blk_height * 16;
        else
            height = 16;
        width = 16;
        dst = dst_start;
        psrc_y = y + in_stride * blk_height * 16;
        psrc_u = u + uin_stride * blk_height * 8;
        psrc_v = v + uin_stride * blk_height * 8;
        for(i = 0; i < block_width; i++)
        {

            if((width != 0) && (height != 0))
            {
                voyuv420torgb32_16nx2n_x86(psrc_y, psrc_u, psrc_v, in_stride, uin_stride, 16, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            psrc_y += width;
            psrc_u += width / 2;
            psrc_v += width / 2;
        }
        break;

    case ARGB32NoResizeNoRot:
        if((blk_height + 1) * 16 > conv_data->nOutHeight)
            height = conv_data->nOutHeight - blk_height * 16;
        else
            height = 16;
        width = 16;
        dst = dst_start;
        psrc_y = y + in_stride * blk_height * 16;
        psrc_u = u + uin_stride * blk_height * 8;
        psrc_v = v + uin_stride * blk_height * 8;
        for(i = 0; i < block_width; i++)
        {

            if((width != 0) && (height != 0))
            {
                voyuv420toargb32_16nx2n_x86(psrc_y, psrc_u, psrc_v, in_stride, uin_stride, 16, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            psrc_y += width;
            psrc_u += width / 2;
            psrc_v += width / 2;
        }
        break;

    }
#else
    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {

            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);

            if((!skip) && (width != 0) && (height != 0))
            {
                cc_yuv420_rgb32_mb_s(width, height, y, u, v,
                                     dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            }
            x_scale_tab += (width * 3 + 1);
            dst += (width << 2);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
#endif

    return VO_ERR_NONE;
}


VOCCRETURNCODE MultiClrConv(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
#if 0
    VO_S32 i;
    for(i = 0; i < cc_hnd->ClrConvEnvP->totBlkLine; i++)
        BlkLineYUVPlanarToRGB32_resize_norotation(cc_hnd->ClrConvEnvP, 0, i);
#else
    ClrConvEnv *ClrConvEnvP = cc_hnd->ClrConvEnvP;
    VO_S32 curBlkLine, curOldBlkLine;

    curOldBlkLine = 0;
    ClrConvEnvP->curBlkLine = 0;
    ClrConvEnvP->completed = 0;
    pthread_cond_broadcast(&ClrConvEnvP->clrconv_cond);
    //     pthread_cond_signal(&ClrConvEnvP->clrconv_cond);
    //     pthread_cond_signal(&ClrConvEnvP->clrconv_cond);
    //     pthread_cond_signal(&ClrConvEnvP->clrconv_cond);

    //vo_sem_wait(&ClrConvEnvP->cc_end_cond);
    //  Sleep(5);
    pthread_mutex_lock(&ClrConvEnvP->clrconv_mutex);
    curBlkLine =  ClrConvEnvP->curBlkLine++;
    while(curBlkLine < ClrConvEnvP->totBlkLine)
    {
        curOldBlkLine = curBlkLine;
        pthread_mutex_unlock(&ClrConvEnvP->clrconv_mutex);
        BlkLineClrConv(ClrConvEnvP, 0, curBlkLine);
        pthread_mutex_lock(&ClrConvEnvP->clrconv_mutex);
        curBlkLine =  ClrConvEnvP->curBlkLine++;
    }

    while(ClrConvEnvP->completed != (ClrConvEnvP->thdNum - 1))
        pthread_cond_wait(&ClrConvEnvP->cc_end_cond, &ClrConvEnvP->clrconv_mutex);
    pthread_mutex_unlock(&ClrConvEnvP->clrconv_mutex);

#if 0
    if(curOldBlkLine !=   ClrConvEnvP->totBlkLine - 1)
    {
        vo_sem_wait(&ClrConvEnvP->cc_end_cond);
    }
#endif

    return VO_ERR_NONE;

#endif
}


#endif

VOCCRETURNCODE YUVPlanarToRGB32_resize_norotation_half(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
#if defined(VOX86)
    VO_S32 block_width	= (conv_data->nOutWidth) / 16;
    VO_S32 block_height = (conv_data->nOutHeight) / 16;
    VO_S32 height_remainder = 0;
    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
#endif

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

#ifdef VOX86
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 16 * 16;
    dst_startv = cc_hnd->pYUVBuffer + 16 * 16 + 16 * 16 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

#if DEBUG_DLL
    do
    {
        FILE *f;
        VO_S8 buff[128];
        f = fopen("D:/cc.log", "a+");
        sprintf(buff, "%s block_width %d block_height %d\n", __FUNCTION__, block_width, block_height);
        fputs(buff, f);
        fclose(f);
    }
    while(0);
#endif


    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = out_buf = conv_data->pOutBuf[0];

#if defined(VOX86)
    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];

        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86_half(width, height, y, u, v,
                                            dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start += (height * out_stride);
    }

    height_remainder = conv_data->nOutHeight - block_height * 16;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
    }
#else
    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {

            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);

            if((!skip) && (width != 0) && (height != 0))
            {
                cc_yuv420_rgb32_mb_s_c_half(width, height, y, u, v,
                                            dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            }
            x_scale_tab += (width * 3 + 1);
            dst += (width << 2);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
#endif

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB32_resize_rotation90l(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
#ifndef VOARMV7
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
#else
    VO_S32 block_width  = (conv_data->nOutWidth + 7) / 8;
    VO_S32 block_height = (conv_data->nOutHeight) / 8;
    VO_S32 height_remainder = 0;

    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
#endif

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_width  = conv_data->nOutWidth;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

#ifdef VOARMV7
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;

#ifdef VOARMV7
    dst_start = out_buf = conv_data->pOutBuf[0] + (out_width - 8) * out_stride;

    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];

        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {

            width = x_scale_tab[0];
            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x8_sL90_armv7(width, height, y, u, v,
                                          dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            x_scale_tab += (width * 3 / 2 + 1);
            dst -= (width * out_stride);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start += (height << 2);
    }

    height_remainder = conv_data->nOutHeight - block_height * 8;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x2n_sL90_armv7(width, height, y, u, v,
                                           dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                cc_rgb32_mb_new(dsty, dstu, dstv, 8, dst, out_stride, height, width, 4, 4);
            }

            dst -= (width * out_stride);
            x_scale_tab += (width * 3 / 2 + 1);
        }
    }
#else
    dst_start = out_buf = conv_data->pOutBuf[0] + (out_width - 1) * out_stride;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_rgb32_mb_rotation_90l_s(width, height, y, u, v,
                                                  dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);

            x_scale_tab += (width * 3 + 1);
            dst -= (width * out_stride);
        }

        dst_start += (height << 2);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
#endif

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB32_resize_rotation90l_half(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 skip = 0;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_width  = conv_data->nOutWidth;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;

    dst_start = out_buf = conv_data->pOutBuf[0] + (out_width - 1) * out_stride;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_rgb32_mb_rotation_90l_s_c_half(width, height, y, u, v,
                        dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);

            x_scale_tab += (width * 3 + 1);
            dst -= (width * out_stride);
        }

        dst_start += (height << 2);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB32_resize_rotation90r(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
#ifndef VOARMV7
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
#else
    VO_S32 block_width  = (conv_data->nOutWidth + 7) / 8;
    VO_S32 block_height = (conv_data->nOutHeight) / 8;
    VO_S32 height_remainder = 0;

    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
#endif

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

#ifdef VOARMV7
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;

#ifdef VOARMV7
    dst_start = out_buf = conv_data->pOutBuf[0] + (conv_data->nOutHeight << 2) - 32;

    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {

            width = x_scale_tab[0];
            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x8_sR90_armv7(width, height, y, u, v,
                                          dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            x_scale_tab += (width * 3 / 2 + 1);
            dst += (width * out_stride);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start -= (height << 2);
    }

    height_remainder = conv_data->nOutHeight - block_height * 8;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start + (8 - height) * 4;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x2n_sR90_armv7(width, height, y, u, v,
                                           dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                cc_rgb32_mb_new(dsty + (8 - height), dstu + ((8 - height) >> 1), dstv + ((8 - height) >> 1), 8, dst, out_stride, height, width, 4, 4);
            }

            dst += (width * out_stride);
            x_scale_tab += (width * 3 / 2 + 1);
        }

    }
#else
    dst_start = out_buf = conv_data->pOutBuf[0] + (conv_data->nOutHeight << 2) - 8;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_rgb32_mb_rotation_90r_s(width, height, y, u, v,
                                                  dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width * out_stride);
        }
        dst_start -= (height << 2);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
#endif

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB32_resize_rotation90r_half(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 skip = 0;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;

    dst_start = out_buf = conv_data->pOutBuf[0] + (conv_data->nOutHeight << 2) - 8;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_rgb32_mb_rotation_90r_s_c_half(width, height, y, u, v,
                        dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width * out_stride);
        }
        dst_start -= (height << 2);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB32_resize_rotation180(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
#ifndef VOARMV7
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
#else
    VO_S32 block_width  = (conv_data->nOutWidth + 7) / 8;
    VO_S32 block_height = (conv_data->nOutHeight) / 8;
    VO_S32 height_remainder = 0;

    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
#endif

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

#ifdef VOARMV7
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;

#ifdef VOARMV7
    height = y_scale_tab[0];
    dst_start = out_buf = conv_data->pOutBuf[0] + out_stride * (conv_data->nOutHeight - height + 1) - 32;

    for(j = 0; j < block_height; j++)
    {
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {

            width = x_scale_tab[0];
            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x8_sR180_armv7(width, height, y, u, v,
                                           dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            x_scale_tab += (width * 3 / 2 + 1);
            dst -= (width << 2);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);

        height = y_scale_tab[0];
        dst_start -= (height * out_stride);
    }

    height_remainder = conv_data->nOutHeight - block_height * 8;

    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;

        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x2n_sR180_armv7(width, height, y, u, v,
                                            dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb32_8nx2n_armv7(dsty + 8 * (8 - height), dstu + (4 >> 1)*(8 - height), dstv + (4 >> 1)*(8 - height), 8,
                                            dst, out_stride, width, height, 4, 4);
            }

            dst -= (width << 2);
            x_scale_tab += (width * 3 / 2 + 1);
        }

    }
#else
    height = y_scale_tab[0];
    dst_start = out_buf = conv_data->pOutBuf[0] + out_stride * (conv_data->nOutHeight - height);

    for(j = 0; j < mb_height; j++)
    {
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_rgb32_mb_rotation_180_s(width, height, y, u, v,
                                                  dst + out_stride - (width << 2), in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst -= (width << 2);
        }
        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);

        height = y_scale_tab[0];
        dst_start -= (height * out_stride);
    }
#endif

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToARGB32_resize_norotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
#if !defined(VOARMV7) && !defined(VOX86)
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
#elif defined(VOARMV7)
    VO_S32 block_width  = (conv_data->nOutWidth + 7) / 8;
    VO_S32 block_height = (conv_data->nOutHeight) / 8;
    VO_S32 height_remainder = 0;
    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
#else
    VO_S32 block_width	= (conv_data->nOutWidth) / 16;
    VO_S32 block_height = (conv_data->nOutHeight) / 16;
    VO_S32 height_remainder = 0;
    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
#endif
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

#if DEBUG_DLL
    do
    {
        FILE *f;
        VO_S8 buff[128];
        f = fopen("D:/cc.log", "a+");
        sprintf(buff, "%s block_width %d block_height %d\n", __FUNCTION__, block_width, block_width);
        fputs(buff, f);
        fclose(f);
    }
    while(0);
#endif

#ifdef VOARMV7
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

#ifdef VOX86
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 16 * 16;
    dst_startv = cc_hnd->pYUVBuffer + 16 * 16 + 16 * 16 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = out_buf = conv_data->pOutBuf[0];

#if defined(VOARMV7)
    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];

        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x8_s_armv7(width, height, y, u, v,
                                       dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420toargb32_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 / 2 + 1);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start += (height * out_stride);
    }

    height_remainder = conv_data->nOutHeight - block_height * 8;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x2n_s_armv7(width, height, y, u, v,
                                        dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420toargb32_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 / 2 + 1);
        }
    }
#elif defined(VOX86)
    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];

        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420toargb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start += (height * out_stride);
    }

    height_remainder = conv_data->nOutHeight - block_height * 16;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420toargb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
    }
#else
    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);

            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_argb32_mb_s(width, height, y, u, v,
                                      dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width << 2);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
#endif

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToARGB32_resize_norotation_half(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
#if defined(VOX86)
    VO_S32 block_width	= (conv_data->nOutWidth) / 16;
    VO_S32 block_height = (conv_data->nOutHeight) / 16;
    VO_S32 height_remainder = 0;
    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
#endif
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;


    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

#if DEBUG_DLL
    do
    {
        FILE *f;
        VO_S8 buff[128];
        f = fopen("D:/cc.log", "a+");
        sprintf(buff, "%s block_width %d block_height %d\n", __FUNCTION__, mb_width, mb_width);
        fputs(buff, f);
        fclose(f);
    }
    while(0);
#endif

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

#ifdef VOX86
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 16 * 16;
    dst_startv = cc_hnd->pYUVBuffer + 16 * 16 + 16 * 16 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif


    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = out_buf = conv_data->pOutBuf[0];
#if defined(VOX86)
    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];

        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86_half(width, height, y, u, v,
                                            dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420toargb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start += (height * out_stride);
    }

    height_remainder = conv_data->nOutHeight - block_height * 16;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_16x16_s_x86(width, height, y, u, v,
                                       dst, in_stride, 16, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420toargb32_16nx2n_x86(dsty, dstu, dstv, 16, 8, width, height, (VO_U32 *)dst, out_stride / 4);
            }

            dst += 4 * width;
            x_scale_tab += (width * 3 + 1);
        }
    }
#else
    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);

            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_argb32_mb_s_c_half(width, height, y, u, v,
                                             dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width << 2);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
#endif

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToARGB32_resize_rotation90l(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
#ifndef VOARMV7
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
#else
    VO_S32 block_width  = (conv_data->nOutWidth + 7) / 8;
    VO_S32 block_height = (conv_data->nOutHeight) / 8;
    VO_S32 height_remainder = 0;

    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
#endif

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_width  = conv_data->nOutWidth;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

#ifdef VOARMV7
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;

#ifdef VOARMV7
    dst_start = out_buf = conv_data->pOutBuf[0] + (out_width - 8) * out_stride;

    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];

        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {

            width = x_scale_tab[0];
            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x8_sL90_armv7(width, height, y, u, v,
                                          dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420toargb32_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            x_scale_tab += (width * 3 / 2 + 1);
            dst -= (width * out_stride);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start += (height << 2);
    }

    height_remainder = conv_data->nOutHeight - block_height * 8;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x2n_sL90_armv7(width, height, y, u, v,
                                           dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                cc_argb32_mb_new(dsty, dstu, dstv, 8, dst, out_stride, height, width, 4, 4);
            }

            dst -= (width * out_stride);
            x_scale_tab += (width * 3 / 2 + 1);
        }
    }
#else
    dst_start = out_buf = conv_data->pOutBuf[0] + (out_width - 1) * out_stride;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_argb32_mb_rotation_90l_s(width, height, y, u, v,
                                                   dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);

            x_scale_tab += (width * 3 + 1);
            dst -= (width * out_stride);
        }

        dst_start += (height << 2);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
#endif

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToARGB32_resize_rotation90l_half(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 skip = 0;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_width  = conv_data->nOutWidth;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;

    dst_start = out_buf = conv_data->pOutBuf[0] + (out_width - 1) * out_stride;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_argb32_mb_rotation_90l_s_c_half(width, height, y, u, v,
                        dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);

            x_scale_tab += (width * 3 + 1);
            dst -= (width * out_stride);
        }

        dst_start += (height << 2);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToARGB32_resize_rotation90r(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
#ifndef VOARMV7
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
#else
    VO_S32 block_width  = (conv_data->nOutWidth + 7) / 8;
    VO_S32 block_height = (conv_data->nOutHeight) / 8;
    VO_S32 height_remainder = 0;

    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
#endif

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

#ifdef VOARMV7
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;

#ifdef VOARMV7
    dst_start = out_buf = conv_data->pOutBuf[0] + (conv_data->nOutHeight << 2) - 32;

    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {

            width = x_scale_tab[0];
            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x8_sR90_armv7(width, height, y, u, v,
                                          dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420toargb32_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            x_scale_tab += (width * 3 / 2 + 1);
            dst += (width * out_stride);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start -= (height << 2);
    }

    height_remainder = conv_data->nOutHeight - block_height * 8;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start + (8 - height) * 4;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x2n_sR90_armv7(width, height, y, u, v,
                                           dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                cc_argb32_mb_new(dsty + (8 - height), dstu + ((8 - height) >> 1), dstv + ((8 - height) >> 1), 8, dst, out_stride, height, width, 4, 4);
            }

            dst += (width * out_stride);
            x_scale_tab += (width * 3 / 2 + 1);
        }

    }
#else
    dst_start = out_buf = conv_data->pOutBuf[0] + (conv_data->nOutHeight << 2) - 8;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_argb32_mb_rotation_90r_s(width, height, y, u, v,
                                                   dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width * out_stride);
        }
        dst_start -= (height << 2);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
#endif

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToARGB32_resize_rotation90r_half(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 skip = 0;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;

    dst_start = out_buf = conv_data->pOutBuf[0] + (conv_data->nOutHeight << 2) - 8;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_argb32_mb_rotation_90r_s_c_half(width, height, y, u, v,
                        dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width * out_stride);
        }
        dst_start -= (height << 2);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToARGB32_resize_rotation180(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_S32 i, j, width, height;
    VO_U8 *mb_skip;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

#ifndef VOARMV7
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
#else
    VO_S32 block_width  = (conv_data->nOutWidth + 7) / 8;
    VO_S32 block_height = (conv_data->nOutHeight) / 8;
    VO_S32 height_remainder = 0;

    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
#endif

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

#ifdef VOARMV7
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;

#ifdef VOARMV7
    height = y_scale_tab[0];
    dst_start = out_buf = conv_data->pOutBuf[0] + out_stride * (conv_data->nOutHeight - height + 1) - 32;

    for(j = 0; j < block_height; j++)
    {
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {

            width = x_scale_tab[0];
            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x8_sR180_armv7(width, height, y, u, v,
                                           dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420toargb32_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            x_scale_tab += (width * 3 / 2 + 1);
            dst -= (width << 2);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);

        height = y_scale_tab[0];
        dst_start -= (height * out_stride);
    }

    height_remainder = conv_data->nOutHeight - block_height * 8;

    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;

        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x2n_sR180_armv7(width, height, y, u, v,
                                            dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420toargb32_8nx2n_armv7(dsty + 8 * (8 - height), dstu + (4 >> 1)*(8 - height), dstv + (4 >> 1)*(8 - height), 8,
                                             dst, out_stride, width, height, 4, 4);
            }

            dst -= (width << 2);
            x_scale_tab += (width * 3 / 2 + 1);
        }

    }
#else
    height = y_scale_tab[0];
    dst_start = out_buf = conv_data->pOutBuf[0] + out_stride * (conv_data->nOutHeight - height);

    for(j = 0; j < mb_height; j++)
    {
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_argb32_mb_rotation_180_s(width, height, y, u, v,
                                                   dst + out_stride - (width << 2), in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst -= (width << 2);
        }
        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);

        height = y_scale_tab[0];
        dst_start -= (height * out_stride);
    }
#endif

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB24_resize(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_S32 vx, vy;
    const VO_S32 in_stride = conv_data->nInStride;
    const VO_S32 out_width = conv_data->nOutWidth;
    const VO_S32 out_height = conv_data->nOutHeight;

    const VO_S32 *param = cc_hnd->yuv2rgbmatrix;
    VO_U8 *out_buf = conv_data->pOutBuf[0];
    VO_U8 *psrcY, *psrcU, *psrcV;
    VO_S32 *y_scal_ptr = y_resize_tab;
    VO_U8 **in_buf = conv_data->pInBuf;
    VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    for(vy = 0; vy < out_height;)
    {
        VO_S32 fill_count;

        //update out_buf
        out_buf = conv_data->pOutBuf[0] + vy * out_stride;

        //check the top extension
        vx = *(y_scal_ptr++);
        if(vx < 0)
        {
            fill_count = -vx;
            out_buf += (fill_count * out_stride);
            vx = *(y_scal_ptr++);
        }
        else
        {
            fill_count = 0;
        }

        vy += fill_count;

        //update input buffer
        psrcY = in_buf[0] + (vx & 0xffff) * in_stride;
        psrcU = in_buf[1] + (((vx & 0xffff) >> 1) * (in_stride >> 1));
        psrcV = in_buf[2] + (((vx & 0xffff) >> 1) * (in_stride >> 1));


        if(!(vx & 0xffff0000)) //high 16 bit for check 2 line once
        {
            vy++;
            for(vx = 0; vx < out_width; )
            {
                VO_S32 a0 = 0, a1 = 0, a2 = 0, a3, a4;
                VO_U32 a6 = 0, a7;

                //0-1
                /*do conversion*/
                a4 = x_resize_tab[vx];
                if(a4 & 0x1) //update a0, a1, a2
                {
                    a3 = psrcV[a4>>17] - 128;
                    a2 = psrcU[a4>>17] - 128;

                    a0 = (a3 * param[0]) >> 20;
                    a1 = (a3 * param[1] + a2 * param[3]) >> 20;
                    a2 = (a2 * param[2]) >> 20;
                }

                if(a4 & 0x2) //update pix0
                {
                    a4 = a4 >> 16;
                    a3 = psrcY[a4];

                    a6  = (ccClip255[a3 + a0]) << 16; //r
                    a6 |= (ccClip255[a3 - a1]) << 8; //g
                    a6 |= ccClip255[a3 + a2];//b
                }
                else
                {
                    a6 = (a6 >> 8); // a6 is last 2 RGB
                }
                vx++;

                a4 = x_resize_tab[vx];

                if(a4 & 0x1) //update a0, a1, a2
                {
                    a3 = psrcV[a4>>17] - 128;
                    a2 = psrcU[a4>>17] - 128;

                    a0 = (a3 * param[0]) >> 20;
                    a1 = (a3 * param[1] + a2 * param[3]) >> 20;
                    a2 = (a2 * param[2]) >> 20;
                }

                if(a4 & 0x2) //update pix1
                {
                    a4 = a4 >> 16;
                    a3 = psrcY[a4];

                    a6 |= (ccClip255[a3 + a2]) << 24; //b

                    a7 = ccClip255[a3 - a1];//g
                    a7 |= (ccClip255[a3 + a0]) << 8; //r
                }
                else
                {
                    a6 |= (a6 << 24);
                    a7 = ((a6 & 0x00ffff00) >> 8);
                }

                vx++;

                *((VO_U32 *)out_buf) = a6;
                out_buf += 4;

                //2-3
                a4 = x_resize_tab[vx];
                if(a4 & 0x1) //update a0, a1, a2
                {
                    a3 = psrcV[a4>>17] - 128;
                    a2 = psrcU[a4>>17] - 128;

                    a0 = (a3 * param[0]) >> 20;
                    a1 = (a3 * param[1] + a2 * param[3]) >> 20;
                    a2 = (a2 * param[2]) >> 20;
                }

                if(a4 & 0x2) //update pix0
                {
                    a4 = a4 >> 16;
                    a3 = psrcY[a4];

                    a7 |= (ccClip255[a3 + a2]) << 16; //b
                    a7 |= (ccClip255[a3 - a1]) << 24; //g
                    a6  = ccClip255[a3 + a0];//r
                }
                else
                {
                    a7 |= (a6 & 0xff000000) >> 8;
                    a7 |= (a7 << 24);
                    a6 = (a7 & 0xff00) >> 8;
                }
                vx++;

                *((VO_U32 *)out_buf) = a7;
                out_buf += 4;

                a4 = x_resize_tab[vx];

                if(a4 & 0x1) //update a0, a1, a2
                {
                    a3 = psrcV[a4>>17] - 128;
                    a2 = psrcU[a4>>17] - 128;

                    a0 = (a3 * param[0]) >> 20;
                    a1 = (a3 * param[1] + a2 * param[3]) >> 20;
                    a2 = (a2 * param[2]) >> 20;
                }

                if(a4 & 0x2) //update pix1
                {
                    a4 = a4 >> 16;
                    a3 = psrcY[a4];

                    a6 |= (ccClip255[a3 + a2]) << 8; //b
                    a6 |= (ccClip255[a3 - a1]) << 16; //g
                    a6 |= (ccClip255[a3 + a0]) << 24; //r
                }
                else
                {
                    a6 |= (a7 & 0xffff0000) >> 8;
                    a6 |= (a6 << 24);
                }
                vx++;

                *((VO_U32 *)out_buf) = a6;
                out_buf += 4;
            }
        }
        else  //2 line together
        {
            vy += 2;
            for(vx = 0; vx < out_width; )
            {
                VO_S32 a0 = 0, a1 = 0, a2 = 0, a3, a4, a5;
                VO_U32 a6 = 0, a7, a8 = 0;

                /*do conversion*/
                //0-1
                a4 = x_resize_tab[vx];
                if(a4 & 0x1) //update a0, a1, a2
                {

                    a3 = psrcV[a4>>17] - 128;
                    a2 = psrcU[a4>>17] - 128;

                    a0 = (a3 * param[0]) >> 20;
                    a1 = (a3 * param[1] + a2 * param[3]) >> 20;
                    a2 = (a2 * param[2]) >> 20;

                }

                if(a4 & 02) //update pix0
                {
                    a4 = a4 >> 16;
                    a3 = psrcY[a4];
                    a4 = psrcY[a4+in_stride];

                    a6  = (ccClip255[a3 + a0]) << 16; //r
                    a6 |= (ccClip255[a3 - a1]) << 8; //g
                    a6 |= ccClip255[a3 + a2];//b

                    a8  = (ccClip255[a4 + a0]) << 16; //r
                    a8 |= (ccClip255[a4 - a1]) << 8; //g
                    a8 |= ccClip255[a4 + a2];//b

                }
                else
                {
                    a6 = a6 >> 8;
                    a8 = a8 >> 8; // a6 is last 2 RGB
                }
                vx++;

                a4 = x_resize_tab[vx];

                if(a4 & 0x1) //update a0, a1, a2
                {
                    a3 = psrcV[a4>>17] - 128;
                    a2 = psrcU[a4>>17] - 128;

                    a0 = (a3 * param[0]) >> 20;
                    a1 = (a3 * param[1] + a2 * param[3]) >> 20;
                    a2 = (a2 * param[2]) >> 20;
                }

                if(a4 & 0x2) //update pix1
                {
                    a4 = a4 >> 16;
                    a3 = psrcY[a4];
                    a4 = psrcY[a4+in_stride];

                    a6 |= (ccClip255[a3 + a2]) << 24; //b
                    a5  = ccClip255[a3 - a1];//g
                    a5 |= (ccClip255[a3 + a0]) << 8; //r

                    a8 |= (ccClip255[a4 + a2]) << 24; //b
                    a7  = ccClip255[a4 - a1];//g
                    a7 |= (ccClip255[a4 + a0]) << 8; //r
                }
                else
                {
                    a6 |= (a6 << 24);
                    a5 = ((a6 & 0x00ffff00) >> 8);
                    a8 |= (a8 << 24);
                    a7 = ((a8 & 0x00ffff00) >> 8);
                }

                vx++;

                *((VO_U32 *)out_buf) = a6;
                *((VO_U32 *)(out_buf + out_stride)) = a8;
                out_buf += 4;

                //2-3
                a4 = x_resize_tab[vx];
                if(a4 & 0x1) //update a0, a1, a2
                {

                    a3 = psrcV[a4>>17] - 128;
                    a2 = psrcU[a4>>17] - 128;

                    a0 = (a3 * param[0]) >> 20;
                    a1 = (a3 * param[1] + a2 * param[3]) >> 20;
                    a2 = (a2 * param[2]) >> 20;

                }

                if(a4 & 02) //update pix0
                {
                    a4 = a4 >> 16;
                    a3 = psrcY[a4];
                    a4 = psrcY[a4+in_stride];

                    a5 |= (ccClip255[a3 + a2]) << 16; //b
                    a5 |= (ccClip255[a3 - a1]) << 24; //g
                    a6  = ccClip255[a3 + a0];//r

                    a7 |= (ccClip255[a4 + a2]) << 16; //b
                    a7 |= (ccClip255[a4 - a1]) << 24; //g
                    a8  = ccClip255[a4 + a0];//r

                }
                else
                {
                    a5 |= (a6 & 0xff000000) >> 8;
                    a5 |= (a5 << 24);
                    a6 = (a5 & 0xff00) >> 8;

                    a7 |= (a8 & 0xff000000) >> 8;
                    a7 |= (a7 << 24);
                    a8 = (a7 & 0xff00) >> 8;
                }
                vx++;

                *((VO_U32 *)out_buf) = a5;
                *((VO_U32 *)(out_buf + out_stride)) = a7;
                out_buf += 4;

                a4 = x_resize_tab[vx];

                if(a4 & 0x1) //update a0, a1, a2
                {
                    a3 = psrcV[a4>>17] - 128;
                    a2 = psrcU[a4>>17] - 128;

                    a0 = (a3 * param[0]) >> 20;
                    a1 = (a3 * param[1] + a2 * param[3]) >> 20;
                    a2 = (a2 * param[2]) >> 20;
                }

                if(a4 & 0x2) //update pix1
                {
                    a4 = a4 >> 16;
                    a3 = psrcY[a4];
                    a4 = psrcY[a4+in_stride];

                    a6 |= (ccClip255[a3 + a2]) << 8; //b
                    a6 |= (ccClip255[a3 - a1]) << 16; //g
                    a6 |= (ccClip255[a3 + a0]) << 24; //r

                    a8 |= (ccClip255[a4 + a2]) << 8; //b
                    a8 |= (ccClip255[a4 - a1]) << 16; //g
                    a8 |= (ccClip255[a4 + a0]) << 24; //r
                }
                else
                {
                    a6 |= (a5 & 0xffff0000) >> 8;
                    a6 |= (a6 << 24);
                    a8 |= (a7 & 0xffff0000) >> 8;
                    a8 |= (a8 << 24);
                }

                vx++;

                *((VO_U32 *)out_buf) = a6;
                *((VO_U32 *)(out_buf + out_stride)) = a8;
                out_buf += 4;
            }
        }

        //roll back the out_buf
        out_buf = out_buf - vx * 3;

        //fill the top extension
        while(fill_count > 0)
        {
            memcpy(out_buf - fill_count * out_stride, out_buf, out_width * 3);
            fill_count--;
        }

        //fill the bottom extension
        vx = *(y_scal_ptr);
        if(vx < 0)
        {
            y_scal_ptr++;
            fill_count = -vx;
        }
        else
        {
            fill_count = 0;
        }

        vy += fill_count;
        out_buf += out_stride;
        while(fill_count)
        {
            memcpy(out_buf + fill_count * out_stride, out_buf, out_width * 3);
            fill_count--;
        }
    }
    return VO_ERR_NONE;
}

#ifdef COLCONVENABLE
VOCCRETURNCODE YUVPlanar444ToRGB16_resize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_S32 i, j, width, height;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    //const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInStride;
    vin_stride = conv_data->nInStride;

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = out_buf = conv_data->pOutBuf[0];

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
                yuv444_to_rgb565_mb_s(width, height, y, u, v,
                                      dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width << 1);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanar422_21ToRGB16_resize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_S32 i, j, width, height;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    //const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInStride;
    vin_stride = conv_data->nInStride;

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = out_buf = conv_data->pOutBuf[0];

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
                yuv422_21_to_rgb565_mb_s(width, height, y, u, v,
                                         dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width << 1);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanar411ToRGB16_resize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_S32 i, j, width, height;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    //const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInStride >> 2;
    vin_stride = conv_data->nInStride >> 2;

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = out_buf = conv_data->pOutBuf[0];

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
                yuv411_to_rgb565_mb_s(width, height, y, u, v,
                                      dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width << 1);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanar411VToRGB16_resize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_S32 i, j, width, height;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    //const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInStride;
    vin_stride = conv_data->nInStride;

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = out_buf = conv_data->pOutBuf[0];

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
                yuv411V_to_rgb565_mb_s(width, height, y, u, v,
                                       dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width << 1);
        }
        dst_start += (height * out_stride);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUV422InterlaceToRGB16_resize(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U32 vx = 0, vy;//, rx = 0xFFFFFFFF, ry = 0xFFFFFFFF;
    const VO_U32 in_stride = conv_data->nInStride, out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    VO_S32 YUV422Id = 0;
    VO_U8 *outbuf, *start_outbuf = NULL;
    VO_S32 *param = cc_hnd->yuv2rgbmatrix;
    VO_U8 *psrc;
    VO_S32 step_y = 0, step_x = 0, out_stride = conv_data->nOutStride;

    VO_U32 y_shift, u_shift, v_shift ;
    VO_U32 y0 = 0, y1 = 2;

    if (conv_data->nInType == VO_COLOR_YUYV422_PACKED || conv_data->nInType == VO_COLOR_YUYV422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_YUYV422_PACKED ? 0 : 4;
        YUV422Id = IDYUYV422 + Y_switch;
    }
    else if (conv_data->nInType == VO_COLOR_YVYU422_PACKED || conv_data->nInType == VO_COLOR_YVYU422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_YVYU422_PACKED ? 0 : 4;
        YUV422Id = IDYVYU422 + Y_switch;
    }
    else if (conv_data->nInType == VO_COLOR_UYVY422_PACKED || conv_data->nInType == VO_COLOR_UYVY422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_UYVY422_PACKED ? 0 : 4;
        YUV422Id = IDUYVY422 + Y_switch;
    }
    else if (conv_data->nInType == VO_COLOR_VYUY422_PACKED || conv_data->nInType == VO_COLOR_VYUY422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_VYUY422_PACKED ? 0 : 4;
        YUV422Id = IDVYUY422 + Y_switch;
    }

    if(YUV422Id >= 4)
    {
        YUV422Id -= 4;
        y0 = 2;
        y1 = 0;
    }
    y_shift = (YUV422Id > 1);
    u_shift = u_shift_tab[YUV422Id];
    v_shift = v_shift_tab[YUV422Id];

    /*start of output address*/
    if(conv_data->nRotationType == ROTATION_DISABLE) /*disable rotation*/
    {
        step_y = 2;
        step_x = out_stride;
        start_outbuf = conv_data->pOutBuf[0];
    }
    else if(conv_data->nRotationType == ROTATION_90L)  /*rotation90L*/
    {
        step_y = -out_stride;
        step_x = 2;
        start_outbuf = conv_data->pOutBuf[0] - (out_width - 1) * step_y;
    }
    else if(conv_data->nRotationType == ROTATION_90R)  /*rotation90R*/
    {
        step_y = out_stride;
        step_x = -2;
        start_outbuf = conv_data->pOutBuf[0] + (out_height * 2) - 2;
    }

    for(vy = 0; vy < out_height; vy++)
    {
        outbuf = start_outbuf;

        psrc = conv_data->pInBuf[0] + y_resize_tab[vy] * in_stride;

        for(vx = 0; vx < out_width; vx += 2)
        {
            VO_S32 a0, a1, a2, a3;
            VO_U8 *src_tmp;
            src_tmp = psrc + (x_resize_tab[vx>>1] << 2);

            a3 = src_tmp[v_shift] - 128;
            a2 = src_tmp[u_shift] - 128;

            a0 = (a3 * param[0]) >> 20;
            a1 = (a3 * param[1] + a2 * param[3]) >> 20;
            a2 = (a2 * param[2]) >> 20;

            a3 = src_tmp[y_shift + y0];

            *((VO_U16 *)outbuf) = (((((ccClip31[(a3 + a0)>>3]) << 6) | (ccClip63[(a3 - a1)>>2]) ) << 5)) | (ccClip31[(a3 + a2)>>3]);

            a3 = src_tmp[y_shift + y1];
            outbuf += step_y;

            *((VO_U16 *)outbuf) = ((((((ccClip31[(a3 + a0)>>3]) << 6) | (ccClip63[(a3 - a1)>>2]) ) << 5)) | (ccClip31[(a3 + a2)>>3]));
            outbuf += step_y;

        }
        start_outbuf += step_x;
    }
    return VO_ERR_NONE;
}

VOCCRETURNCODE YUV422InterlaceToRGB24_resize(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U32 vx = 0, vy;//, rx = 0xFFFFFFFF, ry = 0xFFFFFFFF;
    const VO_U32 in_stride = conv_data->nInStride, out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    VO_U8 *outbuf, *start_outbuf = NULL;
    VO_S32 *param = cc_hnd->yuv2rgbmatrix;
    VO_U8 *psrc;
    VO_S32 step_y = 0, step_x = 0, out_stride = conv_data->nOutStride;
    VO_S32 YUV422Id = 0;
    VO_U32 y_shift, u_shift, v_shift ;
    VO_U32 y0 = 0, y1 = 2;

    if (conv_data->nInType == VO_COLOR_YUYV422_PACKED || conv_data->nInType == VO_COLOR_YUYV422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_YUYV422_PACKED ? 0 : 4;
        YUV422Id = IDYUYV422 + Y_switch;
    }
    else if (conv_data->nInType == VO_COLOR_YVYU422_PACKED || conv_data->nInType == VO_COLOR_YVYU422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_YVYU422_PACKED ? 0 : 4;
        YUV422Id =  IDYVYU422 + Y_switch;
    }
    else if (conv_data->nInType == VO_COLOR_UYVY422_PACKED || conv_data->nInType == VO_COLOR_UYVY422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_UYVY422_PACKED ? 0 : 4;
        YUV422Id =  IDUYVY422 + Y_switch;
    }
    else if (conv_data->nInType == VO_COLOR_VYUY422_PACKED || conv_data->nInType == VO_COLOR_VYUY422_PACKED_2)
    {
        const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_VYUY422_PACKED ? 0 : 4;
        YUV422Id =  IDVYUY422 + Y_switch;
    }

    if(YUV422Id >= 4)
    {
        YUV422Id -= 4;
        y0 = 2;
        y1 = 0;
    }
    y_shift = (YUV422Id > 1);
    u_shift = u_shift_tab[YUV422Id];
    v_shift = v_shift_tab[YUV422Id];

    /*start of output address*/
    if(conv_data->nRotationType == ROTATION_DISABLE) /*disable rotation*/
    {
        step_y = 3;
        step_x = out_stride;
        start_outbuf = conv_data->pOutBuf[0];
    }
    else if(conv_data->nRotationType == ROTATION_90L)  /*rotation90L*/
    {
        step_y = -out_stride;
        step_x = 3;
        start_outbuf = conv_data->pOutBuf[0] - (out_width - 1) * step_y;
    }
    else if(conv_data->nRotationType == ROTATION_90R)  /*rotation90R*/
    {
        step_y = out_stride;
        step_x = -3;
        start_outbuf = conv_data->pOutBuf[0] + (out_height * 3) - 3;
    }

    for(vy = 0; vy < out_height; vy++)
    {
        outbuf = start_outbuf;

        psrc = conv_data->pInBuf[0] + y_resize_tab[vy] * in_stride;

        for(vx = 0; vx < out_width; vx += 2)
        {
            VO_S32 a0, a1, a2, a3;
            VO_U8 *src_tmp;
            src_tmp = psrc + (x_resize_tab[vx>>1] << 2);

            a3 = src_tmp[v_shift] - 128;
            a2 = src_tmp[u_shift] - 128;

            a0 = (a3 * param[0]) >> 20;
            a1 = (a3 * param[1] + a2 * param[3]) >> 20;
            a2 = (a2 * param[2]) >> 20;

            a3 = src_tmp[y_shift + y0];

            *(outbuf)   = (ccClip255[(a3 + a2)]);
            *(outbuf + 1) = (ccClip255[(a3 - a1)]);
            *(outbuf + 2) = (ccClip255[(a3 + a0)]);

            outbuf += step_y;

            a3 = src_tmp[y_shift + y1];

            *(outbuf) = (ccClip255[(a3 + a2)]);
            *(outbuf + 1) = (ccClip255[(a3 - a1)]);
            *(outbuf + 2) = (ccClip255[(a3 + a0)]);
            outbuf += step_y;

        }
        start_outbuf += step_x;
    }
    return VO_ERR_NONE;
}
#endif //COLCONVENABLE

//Rotation
VOCCRETURNCODE YUVPlanarToRGB16_normal_flip(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U32 vx = 0, vy;
    const VO_U32 in_stride = conv_data->nInStride , out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    const VO_S32 *param = cc_hnd->yuv2rgbmatrix;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    //const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);
    VO_U8 *outbuf;
    VO_U8 *psrcY, *psrcU, *psrcV;

    VO_S32 out_stride = conv_data->nOutStride;

    for(vy = 0; vy < out_height; vy += 2)
    {
        outbuf = conv_data->pOutBuf[0] + ((out_height - vy - 1) * out_stride);

        vx =  vy * in_stride;
        psrcY = conv_data->pInBuf[0] + vx;

        psrcU = conv_data->pInBuf[1] + (vx >> 2);
        psrcV = conv_data->pInBuf[2] + (vx >> 2);

        for(vx = 0; vx < out_width;)
        {
            VO_S32 a0, a1, a2, a3, a4, a5;
            a4 = psrcY[vx];
            a3 = psrcV[vx>>1] - 128;
            a2 = psrcU[vx>>1] - 128;

            a0 = (a3 * param[0]) >> 20;
            a1 = (a3 * param[1] + a2 * param[3]) >> 20;
            a2 = (a2 * param[2]) >> 20;

            a3 = psrcY[vx + 1];

            a5  = (ccClip31[(a4 + a0 + RBDITHER_P2)>>3] << 11);
            a5  |= (ccClip63[(a4 - a1 + GDITHER_P0)>>2] << 5);
            a5  |= (ccClip31[(a4 + a2 + RBDITHER_P0)>>3]);

            a4 = psrcY[vx + in_stride];

            a5  |= (ccClip31[(a3 + a0 + RBDITHER_P3)>>3] << 27);
            a5  |= (ccClip63[(a3 - a1 + GDITHER_P1)>>2] << 21);
            a5  |= (ccClip31[(a3 + a2 + RBDITHER_P1)>>3] << 16);

            a3 = psrcY[vx + in_stride + 1];
            *((VO_U32 *)outbuf) = (VO_U32)a5;

            a5  = (ccClip31[(a4 + a0 + RBDITHER_P0)>>3] << 11);
            a5  |= (ccClip63[(a4 - a1 + GDITHER_P2)>>2] << 5);
            a5  |= (ccClip31[(a4 +a2 + RBDITHER_P2)>>3]);

            a5  |= (ccClip31[(a3 + a0 + RBDITHER_P1)>>3] << 27);
            a5  |= (ccClip63[(a3 -  a1 + GDITHER_P3)>>2] << 21);
            a5  |= (ccClip31[(a3 +  a2 + RBDITHER_P3)>>3] << 16);

            *((VO_U32 *)(outbuf - out_stride)) = (VO_U32)a5;
            vx += 2;
            outbuf += 4;
        }

    }
    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB16_noresize(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{

    VO_U32 vx = 0, vy = 0;
    const VO_U32 in_stride = conv_data->nInStride, out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    const VO_S32 out_stride = conv_data->nOutStride;
    VO_U8 *psrc_y, *psrc_u, *psrc_v, *out_buf, *start_out_buf = NULL;
    //VO_S32 *param = cc_hnd->yuv2rgbmatrix;
    VO_S32 step_x = 0, step_y = 0;
    const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

#if defined(VOARMV7)
    VO_U32 uin_stride = conv_data->nInStride >> is420Planar;
    VO_U32 vin_stride = conv_data->nInStride >> is420Planar;
    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;

    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;

    if((out_width < 16) || (out_height < 16))
        goto Normal_convert;

    if(conv_data->nRotationType == ROTATION_90L)
    {
        YUV420PLANARTOYUV420_MB_ROTATION = cc_yuv2yuv_8x8_L90_armv7;
        start_out_buf = conv_data->pOutBuf[0] + (out_width - 8) * out_stride;
        step_x = -(out_stride << 3);
        step_y = 16;
    }
    else if(conv_data->nRotationType == ROTATION_90R)
    {
        YUV420PLANARTOYUV420_MB_ROTATION = cc_yuv2yuv_8x8_R90_armv7;
        start_out_buf = conv_data->pOutBuf[0] + (out_height << 1) - 16;
        step_x = (out_stride << 3);
        step_y = -16;

    }
    else if(conv_data->nRotationType == ROTATION_180)
    {
        YUV420PLANARTOYUV420_MB_ROTATION = cc_yuv2yuv_8x8_R180_armv7;
        start_out_buf = conv_data->pOutBuf[0] + out_stride * (conv_data->nOutHeight - 7) - 16;
        step_x = -16;
        step_y = -(out_stride << 3);
    }

    /*prepare resize*/
    for(vy = 8; vy <= out_height; vy += 8)
    {
        VO_S32 offset = (vy - 8) * in_stride;

        psrc_y = conv_data->pInBuf[0] + offset;
        psrc_u = conv_data->pInBuf[1] + (offset >> (1 + is420Planar));
        psrc_v = conv_data->pInBuf[2] + (offset >> (1 + is420Planar));

        out_buf = start_out_buf;
        for(vx = 8; vx <= out_width; vx += 8)
        {

            YUV420PLANARTOYUV420_MB_ROTATION(psrc_y, psrc_u, psrc_v, in_stride, uin_stride, vin_stride, dsty, dstu, dstv);
            voyuv420torgb16_8nx2n_armv7(dsty, dstu, dstv, 8, out_buf, out_stride, 8, 8, 4, 4);

            psrc_y += 8;
            psrc_u += 4;
            psrc_v += 4;
            out_buf += step_x;

        }
        start_out_buf += step_y;
    }
#else
    VO_U8 *mb_skip;
    VO_S32 skip = 0;
    VO_U32 uin_stride = conv_data->nInUVStride << (!is420Planar);
    VO_U32 vin_stride = conv_data->nInUVStride << (!is420Planar);

    mb_skip = conv_data->mb_skip;

    if((out_width < 16) || (out_height < 16))
        goto Normal_convert;

    if(conv_data->nRotationType == ROTATION_90L)
    {
        YUV420PLANARTORGB16_NEWMB = YUV420PlanarToRGB16_NEWMB_rotation_90L;
        start_out_buf = conv_data->pOutBuf[0] + (out_width - 1) * out_stride;
        step_x = -(out_stride << 4);
        step_y = 32;
    }
    else if(conv_data->nRotationType == ROTATION_90R)
    {
        YUV420PLANARTORGB16_NEWMB = YUV420PlanarToRGB16_NEWMB_rotation_90R;
        start_out_buf = conv_data->pOutBuf[0] + (out_height << 1) - 4;
        step_x = (out_stride << 4);
        step_y = -32;
    }
    else if(conv_data->nRotationType == ROTATION_180)
    {
        YUV420PLANARTORGB16_NEWMB = YUV420PlanarToRGB16_NEWMB_rotation_180;
        //start_out_buf = conv_data->pOutBuf[0] + (out_height* out_stride) - 4;
        start_out_buf = conv_data->pOutBuf[0] + (out_height - 15) * out_stride - 32;
        step_x = -32;
        step_y = -(out_stride << 4);
    }

    /*prepare resize*/
    for(vy = 16; vy <= out_height; vy += 16)
    {
        VO_S32 offset = (vy - 16) * in_stride;

        psrc_y = conv_data->pInBuf[0] + offset;
        offset = ((vy - 16) >> 1) * uin_stride;
        psrc_u = conv_data->pInBuf[1] + offset;
        psrc_v = conv_data->pInBuf[2] + offset;

        out_buf = start_out_buf;
        for(vx = 16; vx <= out_width; vx += 16)
        {
            if(mb_skip) skip = *(mb_skip++);

            if(!skip)
            {
                YUV420PLANARTORGB16_NEWMB(psrc_y, psrc_u, psrc_v, in_stride, out_buf, out_stride, 16, 16, uin_stride, vin_stride);
            }

            psrc_y += 16;
            psrc_u += 8;
            psrc_v += 8;
            out_buf += step_x;

        }
        start_out_buf += step_y;
    }
#endif

#if defined(VOARMV7)
    vx = vx - 8;
    vy = vy - 8;
#else
    vx = vx - 16;
    vy = vy - 16;
#endif

Normal_convert:

    if(vx != out_width) /*not multiple of 16*/
    {

        ClrConvData tmp_conv_data;
        tmp_conv_data = *conv_data;

        tmp_conv_data.nOutWidth = out_width - vx;
        tmp_conv_data.nOutHeight = vy;

        tmp_conv_data.pInBuf[0] = conv_data->pInBuf[0] + vx;
        tmp_conv_data.pInBuf[1] = conv_data->pInBuf[1] + (vx >> 1);
        tmp_conv_data.pInBuf[2] = conv_data->pInBuf[2] + (vx >> 1);
        if(conv_data->nRotationType == ROTATION_90L)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0];
        }
        else if(conv_data->nRotationType == ROTATION_90R)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + out_stride * (vx + 1) - (vy << 1) ;
        }
        else if(conv_data->nRotationType == ROTATION_180)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + ((out_width - vx) << 1) + out_stride * (out_height - 1);
        }
        else
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (vx << 1);
        }

        NewYUVPlanarToRGB16_normal(&tmp_conv_data, isRGB565, is420Planar, cc_hnd);
    }

    if(vy != out_height)
    {
        ClrConvData tmp_conv_data;
        tmp_conv_data = *conv_data;

        tmp_conv_data.pInBuf[0] = conv_data->pInBuf[0] + vy * in_stride;
        tmp_conv_data.pInBuf[1] = conv_data->pInBuf[1] + (vy >> is420Planar) * (in_stride >> 1);
        tmp_conv_data.pInBuf[2] = conv_data->pInBuf[2] + (vy >> is420Planar) * (in_stride >> 1);

        if(conv_data->nRotationType == ROTATION_90L)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (vy << 1);
        }
        else if(conv_data->nRotationType == ROTATION_90R)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0];
        }
        else if(conv_data->nRotationType == ROTATION_180)
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + (out_height - vy) * out_stride;
        }
        else
        {
            tmp_conv_data.pOutBuf[0] = conv_data->pOutBuf[0] + vy * out_stride;
        }

        tmp_conv_data.nOutWidth = out_width;
        tmp_conv_data.nOutHeight = out_height - vy;
        NewYUVPlanarToRGB16_normal(&tmp_conv_data, isRGB565, is420Planar, cc_hnd);
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB16_resize(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_S32 vx, vy;
    const VO_S32 in_stride  = conv_data->nInStride;
    const VO_S32 out_width  = conv_data->nOutWidth;
    const VO_S32 out_height = conv_data->nOutHeight;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    const VO_S32 *param = cc_hnd->yuv2rgbmatrix;
    VO_U8 *out_buf = conv_data->pOutBuf[0];
    VO_U8 *psrcY, *psrcU, *psrcV;
    VO_S32 *y_scal_ptr = y_resize_tab;
    VO_U8 **in_buf = conv_data->pInBuf;
    VO_S32 out_stride = conv_data->nOutStride;

    for(vy = 0; vy < out_height;)
    {
        VO_S32 fill_count;

        //update out_buf
        out_buf = conv_data->pOutBuf[0] + vy * out_stride;

        //check the top extension
        vx = *(y_scal_ptr++);
        if(vx < 0)
        {
            fill_count = -vx;
            out_buf += (fill_count * out_stride);
            vx = *(y_scal_ptr++);
        }
        else
        {
            fill_count = 0;
        }

        vy += fill_count;

        //update input buffer
        psrcY = in_buf[0] + (vx & 0xffff) * in_stride;
        psrcU = in_buf[1] + (((vx & 0xffff) >> 1) * (in_stride >> (1 - !is420Planar)));
        psrcV = in_buf[2] + (((vx & 0xffff) >> 1) * (in_stride >> (1 - !is420Planar)));


        if(!(vx & 0xffff0000)) //high 16 bit for check 2 line once
        {
            vy++;
            for(vx = 0; vx < out_width; )
            {
                VO_S32 a0 = 0, a1 = 0, a2 = 0, a3, a4;
                VO_U32 a6 = 0;

                /*do conversion*/
                a4 = x_resize_tab[vx];
                if(a4 & 0x1) //update a0, a1, a2
                {
                    a3 = psrcV[a4>>17] - 128;
                    a2 = psrcU[a4>>17] - 128;

                    a0 = (a3 * param[0]) >> 20;
                    a1 = (a3 * param[1] + a2 * param[3]) >> 20;
                    a2 = (a2 * param[2]) >> 20;
                }

                if(a4 & 0x2) //update pix0
                {
                    a4 = a4 >> 16;
                    a3 = psrcY[a4];
                    a6  = ccClip31[(a3 + a0 + RBDITHER_P2)>>3];//r
                    a4  = ccClip63[(a3 - a1 + GDITHER_P0)>>2];//g
                    a6 = (a6 << 6) | a4;
                    a4  = ccClip31[(a3 + a2 + RBDITHER_P0)>>3];//b
                    a6 = (a6 << 5) | a4;
                }
                else
                {
                    a6 = (a6 >> 16); // a6 is last 2 RGB
                }
                vx++;

                a4 = x_resize_tab[vx];

                if(a4 & 0x1) //update a0, a1, a2
                {
                    a3 = psrcV[a4>>17] - 128;
                    a2 = psrcU[a4>>17] - 128;

                    a0 = (a3 * param[0]) >> 20;
                    a1 = (a3 * param[1] + a2 * param[3]) >> 20;
                    a2 = (a2 * param[2]) >> 20;
                }

                if(a4 & 0x2) //update pix1
                {
                    a4 = a4 >> 16;
                    a3 = psrcY[a4];
                    a4  = ccClip31[(a3 + a0 + RBDITHER_P3)>>3];//r
                    a6 = a6 | (a4 << 27);
                    a4  = ccClip63[(a3 - a1 + GDITHER_P1)>>2];//g
                    a6 = a6 | (a4 << 21);
                    a4  = ccClip31[(a3 + a2 + RBDITHER_P1)>>3];//b
                    a6 = a6 | (a4 << 16);
                }
                else
                {
                    //				a6 = (a6>>16); // a6 is last 2 RGB
                    a4 = (a6 << 16);
                    a6 = a6 | a4;
                }

                *((int *)out_buf) = a6;
                vx++;
                out_buf += 4;
            }
        }
        else  //2 line together
        {
            vy += 2;
            for(vx = 0; vx < out_width; )
            {
                VO_S32 a0 = 0, a1 = 0, a2 = 0, a3, a4, a5;
                VO_U32 a6 = 0, a8 = 0;

                /*do conversion*/
                a4 = x_resize_tab[vx];
                if(a4 & 0x1) //update a0, a1, a2
                {

                    a3 = psrcV[a4>>17] - 128;
                    a2 = psrcU[a4>>17] - 128;

                    a0 = (a3 * param[0]) >> 20;
                    a1 = (a3 * param[1] + a2 * param[3]) >> 20;
                    a2 = (a2 * param[2]) >> 20;

                }

                if(a4 & 02) //update pix0
                {
                    a4 = a4 >> 16;
                    a3 = psrcY[a4];
                    a4 = psrcY[a4+in_stride];
                    a6  = ccClip31[(a3 + a0 + RBDITHER_P2)>>3];//r
                    a5  = ccClip63[(a3 - a1 + GDITHER_P0)>>2];//g
                    a6 = (a6 << 6) | a5;
                    a5  = ccClip31[(a3 + a2 + RBDITHER_P0)>>3];//b
                    a6 = (a6 << 5) | a5;

                    a8  = ccClip31[(a4 + a0 + RBDITHER_P3)>>3];//r
                    a5  = ccClip63[(a4 - a1 + GDITHER_P1)>>2];//g
                    a8 = (a8 << 6) | a5;
                    a5  = ccClip31[(a4 + a2 + RBDITHER_P1)>>3];//b
                    a8 = (a8 << 5) | a5;


                }
                else
                {
                    a6 = a6 >> 16;
                    a8 = a8 >> 16; // a6 is last 2 RGB
                }
                vx++;

                a4 = x_resize_tab[vx];

                if(a4 & 0x1) //update a0, a1, a2
                {
                    a3 = psrcV[a4>>17] - 128;
                    a2 = psrcU[a4>>17] - 128;

                    a0 = (a3 * param[0]) >> 20;
                    a1 = (a3 * param[1] + a2 * param[3]) >> 20;
                    a2 = (a2 * param[2]) >> 20;
                }

                if(a4 & 0x2) //update pix1
                {
                    a4 = a4 >> 16;
                    a3 = psrcY[a4];
                    a4 = psrcY[a4+in_stride];

                    //					a6 = a6<<16;
                    //					a8 = a8<<16;

                    a5  = ccClip31[(a3 + a0 + RBDITHER_P0)>>3];//r
                    a6 = a6 | (a5 << 27);
                    a5  = ccClip63[(a3 - a1 + GDITHER_P2)>>2];//g
                    a6 = a6 | (a5 << 21);
                    a5  = ccClip31[(a3 + a2 + RBDITHER_P2)>>3];//b
                    a6 = a6 | (a5 << 16);

                    a5  = ccClip31[(a4 + a0 + RBDITHER_P1)>>3];//r
                    a8 = a8 | (a5 << 27);
                    a5  = ccClip63[(a4 - a1 + GDITHER_P3)>>2];//g
                    a8 = a8 | (a5 << 21);
                    a5  = ccClip31[(a4 + a2 + RBDITHER_P3)>>3];//b
                    a8 = a8 | (a5 << 16);

                }
                else
                {
                    a3 = a6 << 16;
                    a4 = a8 << 16;
                    a6 = a6 | a3;
                    a8 = a8 | a4;
                }

                *((int *)out_buf) = a6;
                *((int *)(out_buf + out_stride)) = a8;
                vx++;
                out_buf += 4;
            }
        }

        //		if(vy>716)
        //			vy = vy;
        //roll back the out_buf
        out_buf = out_buf - (vx << 1);


        //fill the top extension
        while(fill_count > 0)
        {
            memcpy(out_buf - fill_count * out_stride, out_buf, out_width << 1);
            fill_count--;
        }

        //fill the bottom extension
        vx = *(y_scal_ptr);
        if(vx < 0)
        {
            y_scal_ptr++;
            fill_count = -vx;
        }
        else
        {
            fill_count = 0;
        }

        vy += fill_count;
        out_buf += out_stride;
        while(fill_count)
        {
            memcpy(out_buf + fill_count * out_stride, out_buf, out_width << 1);
            fill_count--;
        }
    }
    return VO_ERR_NONE;
}


VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_90L(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;
#ifndef VOARMV7
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
#else
    VO_S32 block_width  = (conv_data->nOutWidth + 7) / 8;
    VO_S32 block_height = (conv_data->nOutHeight) / 8;
    VO_S32 height_remainder = 0;

    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
#endif

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_S32 out_height = conv_data->nOutHeight;
    const VO_S32 out_width  = conv_data->nOutWidth;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

#ifdef VOARMV7
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;


#ifdef VOARMV7
    dst_start = out_buf = conv_data->pOutBuf[0] + (out_width - 8) * out_stride;

    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];

        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {

            width = x_scale_tab[0];
            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x8_sL90_armv7(width, height, y, u, v,
                                          dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb16_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            x_scale_tab += (width * 3 / 2 + 1);
            dst -= (width * out_stride);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start += (height << 1);
    }

    height_remainder = conv_data->nOutHeight - block_height * 8;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x2n_sL90_armv7(width, height, y, u, v,
                                           dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                cc_mb(dsty, dstu, dstv, 8, dst, out_stride, height, width, 4, 4);
            }

            dst -= (width * out_stride);
            x_scale_tab += (width * 3 / 2 + 1);
        }
    }
#else
    dst_start = out_buf = conv_data->pOutBuf[0] + (out_width - 1) * out_stride;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_rotation_90l_s(width, height, y, u, v,
                                            dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);

            x_scale_tab += (width * 3 + 1);
            dst -= (width * out_stride);
        }

        dst_start += (height << 1);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
#endif

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_90R(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

#ifndef VOARMV7
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
#else
    VO_S32 block_width  = (conv_data->nOutWidth + 7) / 8;
    VO_S32 block_height = (conv_data->nOutHeight) / 8;
    VO_S32 height_remainder = 0;

    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
#endif

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

#ifdef VOARMV7
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;

#ifdef VOARMV7
    dst_start = out_buf = conv_data->pOutBuf[0] + (conv_data->nOutHeight << 1) - 16;

    for(j = 0; j < block_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {

            width = x_scale_tab[0];
            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x8_sR90_armv7(width, height, y, u, v,
                                          dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb16_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            x_scale_tab += (width * 3 / 2 + 1);
            dst += (width * out_stride);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
        dst_start -= (height << 1);
    }

    height_remainder = conv_data->nOutHeight - block_height * 8;
    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start + (8 - height) * 2;
        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x2n_sR90_armv7(width, height, y, u, v,
                                           dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                cc_mb(dsty + (8 - height), dstu + ((8 - height) >> 1), dstv + ((8 - height) >> 1), 8, dst, out_stride, height, width, 4, 4);
            }

            dst += (width * out_stride);
            x_scale_tab += (width * 3 / 2 + 1);
        }

    }
#else
    dst_start = out_buf = conv_data->pOutBuf[0] + (conv_data->nOutHeight << 1) - 4;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_rotation_90r_s(width, height, y, u, v,
                                            dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width * out_stride);
        }
        dst_start -= (height << 1);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
#endif

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_half_90L(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height, skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 out_stride = conv_data->nOutStride;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = conv_data->pOutBuf[0] + (conv_data->nOutWidth - 1) * out_stride;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_s_l90_c_half(width, height, y, u, v,
                                          dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst -= (width * out_stride);
        }
        dst_start += (height << 1);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_half_90R(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height, skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 out_stride = conv_data->nOutStride;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = conv_data->pOutBuf[0] + (conv_data->nOutHeight << 1) - 4;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_s_r90_c_half(width, height, y, u, v,
                                          dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width * out_stride);
        }
        dst_start -= (height << 1);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
    return VO_ERR_NONE;

}

VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_3quarter_90L(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height, skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 out_stride = conv_data->nOutStride;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = conv_data->pOutBuf[0] + (conv_data->nOutWidth - 1) * out_stride;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_s_l90_c_3quarter(width, height, y, u, v,
                                              dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst -= (width * out_stride);
        }
        dst_start += (height << 1);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;
}

VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_3quarter_90R(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height, skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 out_stride = conv_data->nOutStride;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = conv_data->pOutBuf[0] + (conv_data->nOutHeight << 1) - 4;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_s_r90_c_3quarter(width, height, y, u, v,
                                              dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width * out_stride);
        }
        dst_start -= (height << 1);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
    return VO_ERR_NONE;

}

VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_1point5_90L(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height, skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 out_stride = conv_data->nOutStride;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = conv_data->pOutBuf[0] + (conv_data->nOutWidth - 1) * out_stride;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_s_l90_c_1point5(width, height, y, u, v,
                                             dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst -= (width * out_stride);
        }
        dst_start += (height << 1);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
    return VO_ERR_NONE;

}

VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_1point5_90R(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height, skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 out_stride = conv_data->nOutStride;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = conv_data->pOutBuf[0] + (conv_data->nOutHeight << 1) - 4;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_s_r90_c_1point5(width, height, y, u, v,
                                             dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width * out_stride);
        }
        dst_start -= (height << 1);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;

}

VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_double_90L(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height, skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 out_stride = conv_data->nOutStride;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = conv_data->pOutBuf[0] + (conv_data->nOutWidth - 1) * out_stride;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_s_l90_c_double(width, height, y, u, v,
                                            dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst -= (width * out_stride);
        }
        dst_start += (height << 1);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }
    return VO_ERR_NONE;
}


VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_double_90R(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height, skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
    VO_S32 out_stride = conv_data->nOutStride;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;
    dst_start = conv_data->pOutBuf[0] + (conv_data->nOutHeight << 1) - 4;

    for(j = 0; j < mb_height; j++)
    {
        height = y_scale_tab[0];
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_s_r90_c_double(width, height, y, u, v,
                                            dst, in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst += (width * out_stride);
        }
        dst_start -= (height << 1);

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);
    }

    return VO_ERR_NONE;

}

VOCCRETURNCODE YUVPlanarToRGB16_resize_Rotation_180(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U8 *dst_start, *dst, *out_buf;
    VO_U8 *y, *u, *v;
    VO_U8 *mb_skip;
    VO_S32 i, j, width, height;
    VO_S32 *x_scale_tab, *y_scale_tab;
    VO_S32 in_stride, uin_stride, vin_stride;

#ifndef VOARMV7
    VO_S32 skip = 0;
    VO_S32 mb_width = (conv_data->nInWidth + 15) / 16;
    VO_S32 mb_height = (conv_data->nInHeight + 15) / 16;
#else
    VO_S32 block_width  = (conv_data->nOutWidth + 7) / 8;
    VO_S32 block_height = (conv_data->nOutHeight) / 8;
    VO_S32 height_remainder = 0;

    VO_U8 *dst_starty, *dst_startu, *dst_startv, *dsty, *dstu, *dstv;
    //VO_U8 *pYUVBuffer = NULL;
#endif

    const VO_S32 out_stride = conv_data->nOutStride;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    y = conv_data->pInBuf[0];
    u = conv_data->pInBuf[1];
    v = conv_data->pInBuf[2];

    mb_skip    = conv_data->mb_skip;
    in_stride  = conv_data->nInStride;
    uin_stride = conv_data->nInUVStride << (!is420Planar);
    vin_stride = conv_data->nInUVStride << (!is420Planar);

#ifdef VOARMV7
    dst_starty = cc_hnd->pYUVBuffer;
    dst_startu = cc_hnd->pYUVBuffer + 8 * 8;
    dst_startv = cc_hnd->pYUVBuffer + 8 * 8 + 8 * 8 / 4;
    dsty = dst_starty;
    dstu = dst_startu;
    dstv = dst_startv;
#endif

    x_scale_tab = x_resize_tab;
    y_scale_tab = y_resize_tab;


#ifdef VOARMV7
    height = y_scale_tab[0];
    dst_start = out_buf = conv_data->pOutBuf[0] + out_stride * (conv_data->nOutHeight - height + 1) - 16;

    for(j = 0; j < block_height; j++)
    {
        dst = dst_start;
        for(i = 0; i < block_width; i++)
        {

            width = x_scale_tab[0];
            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x8_sR180_armv7(width, height, y, u, v,
                                           dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb16_8nx2n_armv7(dsty, dstu, dstv, 8, dst, out_stride, width, height, 4, 4);
            }

            x_scale_tab += (width * 3 / 2 + 1);
            dst -= (width << 1);
        }

        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);

        height = y_scale_tab[0];
        dst_start -= (height * out_stride);
    }

    height_remainder = conv_data->nOutHeight - block_height * 8;

    if (height_remainder)
    {

        height = y_scale_tab[0];
        dst = dst_start;

        for (i = 0; i < block_width; i++)
        {
            width = x_scale_tab[0];

            if((width != 0) && (height != 0))
            {
                cc_yuv2yuv_8x2n_sR180_armv7(width, height, y, u, v,
                                            dst, in_stride, 8, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride, dsty, dstu, dstv);
                voyuv420torgb16_8nx2n_armv7(dsty + 8 * (8 - height), dstu + (4 >> 1)*(8 - height), dstv + (4 >> 1)*(8 - height), 8,
                                            dst, out_stride, width, height, 4, 4);
            }

            dst -= (width << 1);
            x_scale_tab += (width * 3 / 2 + 1);
        }

    }
#else
    height = y_scale_tab[0];
    dst_start = out_buf = conv_data->pOutBuf[0] + out_stride * (conv_data->nOutHeight - height);

    for(j = 0; j < mb_height; j++)
    {
        dst = dst_start;
        for(i = 0; i < mb_width; i++)
        {
            width = x_scale_tab[0];
            if(mb_skip) skip = *(mb_skip++);
            if((!skip) && (width != 0) && (height != 0))
                cc_yuv420_mb_rotation_180_s(width, height, y, u, v,
                                            dst + out_stride - (width << 1), in_stride, out_stride, x_scale_tab + 1, y_scale_tab + 1, uin_stride, vin_stride);
            x_scale_tab += (width * 3 + 1);
            dst -= (width << 1);
        }
        x_scale_tab = x_resize_tab;
        y_scale_tab += (height * 3 + 1);

        height = y_scale_tab[0];
        dst_start -= (height * out_stride);
    }
#endif

    return VO_ERR_NONE;
}

#define GETYUV(srcData, srcStride, dst, XTAB, YTAB, nMaxHeight) {\
	int ij;\
	a1 = 0;		\
	if(antialias_level < 3)	{			\
	for(ij=0; ij<5;ij++){   \
	int a2YTAB;\
	a2 = ij*3;                 \
	a2YTAB = YTAB[a2];              \
	if(a2YTAB>=nMaxHeight-1){a2YTAB = nMaxHeight-2;};         \
	psrc1 = srcData + a2YTAB * srcStride;                                                              \
	aa1 = YTAB[1+a2];                                                                                     \
	bb1 = YTAB[2+a2];                                                                                      \
	a0 = XTAB[0];                                                                                                  \
	if(a0>=srcStride-1){a0 = srcStride-2;};    \
	aa = XTAB[1];                                                                                                  \
	bb = XTAB[2];                                                                                                  \
	a1 += ((((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+srcStride]*bb + psrc1[a0+srcStride+1]*aa)*aa1)>>20)&0x0FF);       \
	XTAB += 3;};                   \
	if(antialias_level==1||antialias_level == 2){\
	dst = a1 / 5;                                                                              \
	}} \
	else{\
	int ki,li;\
	a0 = 0;aa=0;             \
	for(ki = 0;ki<15;ki++)	{  if(YTAB[ki]==-1 || YTAB[ki]>=nMaxHeight){break;};                       \
	for(li = 0;li<15;li++){                    \
	if(XTAB[li]==-1 || XTAB[li]>=(VO_S32)srcStride ){break;}                   \
{a0+=(VO_U8)*(srcData + YTAB[ki] * srcStride + XTAB[li]); aa++;};} }              \
	if(aa>0) {dst = a0/aa; } else {dst = 0;};XTAB += 15;};\
}

VOCCRETURNCODE YUVPlanarToRGB16_antiAlias_resize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    const VO_U32 is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);
    VO_U32 vx, vy;
    VO_U32 in_stride = conv_data->nInStride;
    VO_U32 uv_stride;
    VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    VO_S32 *y_taby, *y_tabuv;
    VO_S32 *pRGB;
    VO_S32 out_stride = conv_data->nOutStride;
    const VO_S32 nEach = 15;
    const VO_S32 antialias_level = conv_data->nAntiAliasLevel;

    VO_U8 *pYData = conv_data->pInBuf[0];
    VO_U8 *pUData = conv_data->pInBuf[1];
    VO_U8 *pVData = conv_data->pInBuf[2];

    VO_S32 nY = 0, nU = 0, nV = 0;


    uv_stride = conv_data->nInUVStride << (!is420Planar);
    y_taby  = y_resize_tab;
    y_tabuv = y_resize_tab;

    for (vy = 0; vy < out_height; vy++)
    {

        VO_S32 *x_taby = x_resize_tab;
        VO_S32	*x_tabu = x_resize_tab;
        VO_S32 *x_tabv = x_resize_tab;

        pRGB = (VO_S32 *)(conv_data->pOutBuf[0] + vy * out_stride);

        for (vx = 0; vx < out_width; vx += 2)
        {

            VO_U32 a0, a1, a2;
            VO_S32 b0, b1, b2, b4, b5;
            VO_S32 aa, bb, aa1, bb1;
            VO_U8 *psrc1;

            GETYUV(pYData, in_stride, nY, x_taby, y_taby, conv_data->nInHeight)
            GETYUV(pUData, uv_stride, nU, x_tabu, y_tabuv, conv_data->nInHeight / 2)
            GETYUV(pVData, uv_stride, nV, x_tabv, y_tabuv, conv_data->nInHeight / 2)

            nU -= 128;
            nV -= 128;

            b0 = (nV * ConstV1);
            b1 = (nV * ConstV2 + nU * ConstU2);
            b2 = (nU * ConstU1);
            b4 = (nY - 16) * ConstY;

            b4 = ((ccClip63[(((b4 + b0)>>20) + RDITHERNEW_P0)>>2] >> 1) << 11) | ((ccClip63[(((b4 - b1)>>20) + GDITHERNEW_P0)>>2]) << 5) | (ccClip63[(((b4 + b2)>>20) + BDITHERNEW_P0)>>2] >> 1);

            GETYUV(pYData, in_stride, nY, x_taby, y_taby, conv_data->nInHeight)

            b5 = (nY - 16) * ConstY;
            b5 = ((ccClip63[(((b5 + b0)>>20) + RDITHERNEW_P2)>>2] >> 1) << 11) | ((ccClip63[(((b5 - b1)>>20) + GDITHERNEW_P2)>>2]) << 5) | (ccClip63[(((b5 + b2)>>20) + BDITHERNEW_P2)>>2] >> 1);

            b4 |= (b5 << 16);

            *pRGB++ = b4;
        }
        y_taby += nEach;

        if ((vy & 1) == 1)
        {
            y_tabuv += nEach;
        }
    }

    return VO_ERR_NONE;

}

#ifdef COLCONVENABLE
VOCCRETURNCODE YUV444PlanarToRGB16_antiAlias_resize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U32 vx, vy;
    VO_U32 in_stride = conv_data->nInStride;
    VO_U32 uv_stride = conv_data->nInStride;
    VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    VO_S32 *y_taby, *y_tabuv;
    VO_S32 *pRGB;
    VO_S32 out_stride = conv_data->nOutStride;
    const VO_S32 nEach = 15;
    const VO_S32 antialias_level = conv_data->nAntiAliasLevel;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    //const VO_U32 is420Planar =  (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    VO_U8 *pYData = conv_data->pInBuf[0];
    VO_U8 *pUData = conv_data->pInBuf[1];
    VO_U8 *pVData = conv_data->pInBuf[2];

    VO_S32 nY = 0, nU = 0, nV = 0;


    y_taby  = y_resize_tab;
    y_tabuv = y_resize_tab;

    for (vy = 0; vy < out_height; vy++)
    {

        VO_S32 *x_taby = x_resize_tab;
        VO_S32	*x_tabu = x_resize_tab;
        VO_S32 *x_tabv = x_resize_tab;

        pRGB = (VO_S32 *)(conv_data->pOutBuf[0] + vy * out_stride);

        for (vx = 0; vx < out_width; vx += 2)
        {

            VO_U32 a0, a1, a2;
            VO_S32 b0, b1, b2, b4, b5;
            VO_S32 aa, bb, aa1, bb1;
            VO_U8 *psrc1;

            GETYUV(pYData, in_stride, nY, x_taby, y_taby, conv_data->nInHeight)
            GETYUV(pUData, uv_stride, nU, x_tabu, y_tabuv, conv_data->nInHeight)
            GETYUV(pVData, uv_stride, nV, x_tabv, y_tabuv, conv_data->nInHeight)

            nU -= 128;
            nV -= 128;

            b0 = (nV * ConstV1);
            b1 = (nV * ConstV2 + nU * ConstU2);
            b2 = (nU * ConstU1);
            b4 = (nY - 16) * ConstY;

            b4 = ((ccClip63[(((b4 + b0)>>20) + RDITHERNEW_P0)>>2] >> 1) << 11) | ((ccClip63[(((b4 - b1)>>20) + GDITHERNEW_P0)>>2]) << 5) | (ccClip63[(((b4 + b2)>>20) + BDITHERNEW_P0)>>2] >> 1);

            GETYUV(pYData, in_stride, nY, x_taby, y_taby, conv_data->nInHeight)
            GETYUV(pUData, uv_stride, nU, x_tabu, y_tabuv, conv_data->nInHeight)
            GETYUV(pVData, uv_stride, nV, x_tabv, y_tabuv, conv_data->nInHeight)

            nU -= 128;
            nV -= 128;

            b0 = (nV * ConstV1);
            b1 = (nV * ConstV2 + nU * ConstU2);
            b2 = (nU * ConstU1);
            b5 = (nY - 16) * ConstY;
            b5 = ((ccClip63[(((b5 + b0)>>20) + RDITHERNEW_P2)>>2] >> 1) << 11) | ((ccClip63[(((b5 - b1)>>20) + GDITHERNEW_P2)>>2]) << 5) | (ccClip63[(((b5 + b2)>>20) + BDITHERNEW_P2)>>2] >> 1);

            b4 |= (b5 << 16);

            *pRGB++ = b4;
        }
        y_taby  += nEach;
        y_tabuv += nEach;
    }

    return VO_ERR_NONE;

}

VOCCRETURNCODE YUV422_21PlanarToRGB16_antiAlias_resize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U32 vx, vy;
    VO_U32 in_stride = conv_data->nInStride;
    VO_U32 uv_stride = conv_data->nInStride;
    VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    VO_S32 *y_taby, *y_tabuv;
    VO_S32 *pRGB;
    VO_S32 out_stride = conv_data->nOutStride;
    const VO_S32 nEach = 15;
    const VO_S32 antialias_level = conv_data->nAntiAliasLevel;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    //const VO_U32 is420Planar =  (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    VO_U8 *pYData = conv_data->pInBuf[0];
    VO_U8 *pUData = conv_data->pInBuf[1];
    VO_U8 *pVData = conv_data->pInBuf[2];

    VO_S32 nY = 0, nU = 0, nV = 0;


    y_taby  = y_resize_tab;
    y_tabuv = y_resize_tab;

    for (vy = 0; vy < out_height; vy++)
    {

        VO_S32 *x_taby = x_resize_tab;
        VO_S32	*x_tabu = x_resize_tab;
        VO_S32 *x_tabv = x_resize_tab;

        pRGB = (VO_S32 *)(conv_data->pOutBuf[0] + vy * out_stride);

        for (vx = 0; vx < out_width; vx += 2)
        {

            VO_U32 a0, a1, a2;
            VO_S32 b0, b1, b2, b4, b5;
            VO_S32 aa, bb, aa1, bb1;
            VO_U8 *psrc1;

            GETYUV(pYData, in_stride, nY, x_taby, y_taby, conv_data->nInHeight)
            GETYUV(pUData, uv_stride, nU, x_tabu, y_tabuv, conv_data->nInHeight)
            GETYUV(pVData, uv_stride, nV, x_tabv, y_tabuv, conv_data->nInHeight)

            nU -= 128;
            nV -= 128;

            b0 = (nV * ConstV1);
            b1 = (nV * ConstV2 + nU * ConstU2);
            b2 = (nU * ConstU1);
            b4 = (nY - 16) * ConstY;

            b4 = ((ccClip63[(((b4 + b0)>>20) + RDITHERNEW_P0)>>2] >> 1) << 11) | ((ccClip63[(((b4 - b1)>>20) + GDITHERNEW_P0)>>2]) << 5) | (ccClip63[(((b4 + b2)>>20) + BDITHERNEW_P0)>>2] >> 1);

            GETYUV(pYData, in_stride, nY, x_taby, y_taby, conv_data->nInHeight)
            GETYUV(pUData, uv_stride, nU, x_tabu, y_tabuv, conv_data->nInHeight)
            GETYUV(pVData, uv_stride, nV, x_tabv, y_tabuv, conv_data->nInHeight)

            nU -= 128;
            nV -= 128;

            b0 = (nV * ConstV1);
            b1 = (nV * ConstV2 + nU * ConstU2);
            b2 = (nU * ConstU1);
            b5 = (nY - 16) * ConstY;
            b5 = ((ccClip63[(((b5 + b0)>>20) + RDITHERNEW_P2)>>2] >> 1) << 11) | ((ccClip63[(((b5 - b1)>>20) + GDITHERNEW_P2)>>2]) << 5) | (ccClip63[(((b5 + b2)>>20) + BDITHERNEW_P2)>>2] >> 1);

            b4 |= (b5 << 16);

            *pRGB++ = b4;
        }
        y_taby  += nEach;

        if ((vy & 1) == 1)
        {
            y_tabuv += nEach;
        }

    }

    return VO_ERR_NONE;

}

VOCCRETURNCODE YUV411PlanarToRGB16_antiAlias_resize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U32 vx, vy;
    VO_U32 in_stride = conv_data->nInStride;
    VO_U32 uv_stride = conv_data->nInStride >> 2;
    VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    VO_S32 *y_taby;
    VO_S32 *pRGB;
    VO_S32 out_stride = conv_data->nOutStride;
    const VO_S32 nEach = 15;
    const VO_S32 antialias_level = conv_data->nAntiAliasLevel;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    //const VO_U32 is420Planar =  (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    VO_U8 *pYData = conv_data->pInBuf[0];
    VO_U8 *pUData = conv_data->pInBuf[1];
    VO_U8 *pVData = conv_data->pInBuf[2];

    VO_S32 nY = 0, nU = 0, nV = 0;


    y_taby  = y_resize_tab;

    for (vy = 0; vy < out_height; vy++)
    {

        VO_S32 *x_taby = x_resize_tab;

        pRGB = (VO_S32 *)(conv_data->pOutBuf[0] + vy * out_stride);
        pUData = conv_data->pInBuf[1] + y_taby[0] * uv_stride;
        pVData = conv_data->pInBuf[2] + y_taby[0] * uv_stride;

        for (vx = 0; vx < out_width; vx += 2)
        {

            VO_U32 a0, a1, a2;
            VO_S32 b0, b1, b2, b4, b5;
            VO_S32 xuv_out_pos;
            VO_S32 aa, bb, aa1, bb1;
            VO_U8 *psrc1;

            xuv_out_pos = ((x_taby[0] + 2) >> 2);

            GETYUV(pYData, in_stride, nY, x_taby, y_taby, conv_data->nInHeight)

            nU = pUData[xuv_out_pos];
            nV = pVData[xuv_out_pos];

            nU -= 128;
            nV -= 128;

            b0 = (nV * ConstV1);
            b1 = (nV * ConstV2 + nU * ConstU2);
            b2 = (nU * ConstU1);
            b4 = (nY - 16) * ConstY;

            b4 = ((ccClip63[(((b4 + b0)>>20) + RDITHERNEW_P0)>>2] >> 1) << 11) | ((ccClip63[(((b4 - b1)>>20) + GDITHERNEW_P0)>>2]) << 5) | (ccClip63[(((b4 + b2)>>20) + BDITHERNEW_P0)>>2] >> 1);

            GETYUV(pYData, in_stride, nY, x_taby, y_taby, conv_data->nInHeight)

            b5 = (nY - 16) * ConstY;
            b5 = ((ccClip63[(((b5 + b0)>>20) + RDITHERNEW_P2)>>2] >> 1) << 11) | ((ccClip63[(((b5 - b1)>>20) + GDITHERNEW_P2)>>2]) << 5) | (ccClip63[(((b5 + b2)>>20) + BDITHERNEW_P2)>>2] >> 1);

            b4 |= (b5 << 16);

            *pRGB++ = b4;
        }
        y_taby  += nEach;
    }

    return VO_ERR_NONE;

}

VOCCRETURNCODE YUV411VPlanarToRGB16_antiAlias_resize_noRotation(ClrConvData *conv_data, VO_S32 *const x_resize_tab, VO_S32 *const y_resize_tab, struct CC_HND *cc_hnd)
{
    VO_U32 vx, vy;
    VO_U32 in_stride = conv_data->nInStride;
    VO_U32 uv_stride = conv_data->nInStride;
    VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
    VO_S32 *y_taby;
    VO_S32 *pRGB;
    VO_S32 out_stride = conv_data->nOutStride;
    const VO_S32 nEach = 15;
    const VO_S32 antialias_level = conv_data->nAntiAliasLevel;
    //const VO_U32 isRGB565 = (conv_data->nOutType == VO_COLOR_RGB565_PACKED);
    //const VO_U32 is420Planar =  (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    VO_U8 *pYData = conv_data->pInBuf[0];
    VO_U8 *pUData = conv_data->pInBuf[1];
    VO_U8 *pVData = conv_data->pInBuf[2];

    VO_S32 nY = 0, nU = 0, nV = 0;


    y_taby  = y_resize_tab;

    for (vy = 0; vy < out_height; vy++)
    {

        VO_S32 *x_taby = x_resize_tab;

        pRGB = (VO_S32 *)(conv_data->pOutBuf[0] + vy * out_stride);
        pUData = conv_data->pInBuf[1] + (y_taby[0] >> 2) * uv_stride;
        pVData = conv_data->pInBuf[2] + (y_taby[0] >> 2) * uv_stride;

        for (vx = 0; vx < out_width; vx += 2)
        {

            VO_U32 a0, a1, a2;
            VO_S32 b0, b1, b2, b4, b5;
            VO_S32 aa, bb, aa1, bb1;
            VO_S32 xuv_out_pos;
            VO_U8 *psrc1;

            xuv_out_pos = x_taby[0];
            GETYUV(pYData, in_stride, nY, x_taby, y_taby, conv_data->nInHeight)


            nU = pUData[xuv_out_pos];
            nV = pVData[xuv_out_pos];
            nU -= 128;
            nV -= 128;

            b0 = (nV * ConstV1);
            b1 = (nV * ConstV2 + nU * ConstU2);
            b2 = (nU * ConstU1);
            b4 = (nY - 16) * ConstY;

            b4 = ((ccClip63[(((b4 + b0)>>20) + RDITHERNEW_P0)>>2] >> 1) << 11) | ((ccClip63[(((b4 - b1)>>20) + GDITHERNEW_P0)>>2]) << 5) | (ccClip63[(((b4 + b2)>>20) + BDITHERNEW_P0)>>2] >> 1);

            xuv_out_pos = x_taby[0];

            GETYUV(pYData, in_stride, nY, x_taby, y_taby, conv_data->nInHeight)


            nU = pUData[xuv_out_pos];
            nV = pVData[xuv_out_pos];
            nU -= 128;
            nV -= 128;

            b0 = (nV * ConstV1);
            b1 = (nV * ConstV2 + nU * ConstU2);
            b2 = (nU * ConstU1);
            b5 = (nY - 16) * ConstY;
            b5 = ((ccClip63[(((b5 + b0)>>20) + RDITHERNEW_P2)>>2] >> 1) << 11) | ((ccClip63[(((b5 - b1)>>20) + GDITHERNEW_P2)>>2]) << 5) | (ccClip63[(((b5 + b2)>>20) + BDITHERNEW_P2)>>2] >> 1);

            b4 |= (b5 << 16);

            *pRGB++ = b4;
        }

        y_taby  += nEach;
    }

    return VO_ERR_NONE;

}

#endif //COLCONVENABLE


#ifdef VOX86
THREAD_FUNCTION ClrConvProc(void *para)
{
    ClrConvEnv *ClrConvEnvP = (ClrConvEnv *)para;
    VO_S32       thdIdx = ClrConvEnvP->thdIdx;
    VO_S32       curBlkLine ;
    VO_U8        buff[128];

    pthread_mutex_lock(&ClrConvEnvP->clrconv_mutex);
    ClrConvEnvP->bRunning = 1;
    // 	sem_post(&ClrConvEnvP->cc_end_cond);

    while(1)
    {
#if DEBUG_DLL
        //    	      printf("thread %d\n", thdIdx);
        sprintf(buff, "thread %d\n", thdIdx);
        DEBUG(buff);
#endif
        pthread_cond_wait(&ClrConvEnvP->clrconv_cond, &ClrConvEnvP->clrconv_mutex);
        if(ClrConvEnvP->bExit)
            break;
        curBlkLine =  ClrConvEnvP->curBlkLine++;
        while(curBlkLine < ClrConvEnvP->totBlkLine)
        {
#if DEBUG_DLL
            sprintf(buff, "thread %d %d\n", thdIdx, curBlkLine);
            DEBUG(buff);
#endif
            //	      DEBUG("thread %d %d\n", thdIdx, curBlkLine);
            pthread_mutex_unlock(&ClrConvEnvP->clrconv_mutex);
            BlkLineClrConv(ClrConvEnvP, thdIdx, curBlkLine);
            pthread_mutex_lock(&ClrConvEnvP->clrconv_mutex);
#if 0
            if(curBlkLine ==  ClrConvEnvP->totBlkLine - 1)
            {
                sprintf(buff, "send thread %d %d\n", thdIdx, curBlkLine);
                DEBUG(buff);

                //	              printf("thread %d %d\n", thdIdx, curBlkLine);
                vo_sem_post(&ClrConvEnvP->cc_end_cond);
            }
#endif
            curBlkLine =  ClrConvEnvP->curBlkLine++;

        }
        ClrConvEnvP->completed++;
        pthread_cond_signal(&ClrConvEnvP->cc_end_cond);
    }
    ClrConvEnvP->completed++;
    pthread_cond_signal(&ClrConvEnvP->cc_end_cond);
    pthread_mutex_unlock(&ClrConvEnvP->clrconv_mutex);

}
#endif
