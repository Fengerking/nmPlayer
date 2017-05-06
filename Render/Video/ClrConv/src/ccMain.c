/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2006				*
*																		*
************************************************************************/

#include "ccInternal.h"
#include "ccYUVToYUV.h"
#include "ccYUVToRGB.h"
#include "ccRGBToRGB.h"
#include "ccRGBToRGBMB.h"
#include <memory.h>
#include <stdlib.h>
#include <string.h>

#ifdef TIME_ANDROID
#include <stdio.h>
#include <sys/time.h>
#endif

#if defined(TIME_ANDROID)//&&!defined(NDK_BUILD)
//#include <pthread.h>
#ifdef NDK_BUILD
#include <android/log.h>
#define LOG_TAG "_VOH264"
#define LOGI(...) ((int)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#else//NDK_BUILD
#include <utils/Log.h>
#endif//NDK_BUILD
#define LOG_TAG "_VOH264"
#else//_LINUX_ANDROID
#define LOGI(...)
#endif//_LINUX_ANDROID

#define DEBUG_DLL 0


#if defined (_IOS) || defined(_MAC_OS) || defined(TIME_ANDROID)
unsigned long start = 0, nframes = 0, total = 0;

static unsigned long GetTickCount()
{
#if 1
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return tval.tv_sec * 1000 + tval.tv_usec / 1000;
#else
    return clock();
#endif
}
#endif

//#define CC_LOGINFO
#ifdef CC_LOGINFO
#define LOG_TAG "voMMCCRRS"
#include "voLog.h"
#endif // CC_LOGINFO


#define CACHE_LINE 64
#define SAFETY 32
#define SCALE_CONST 1024
extern VO_S32 param_tab[3][4];

#if 0
#define ALIGN_MEM(out_mem){\
	VO_U32 tmp = (VO_U32)out_mem;\
	if(tmp & ~(CACHE_LINE - 1)){\
	tmp += (CACHE_LINE - 1);\
	tmp = (tmp & (~(CACHE_LINE - 1)));\
	}\
	out_mem = (VO_U8*)tmp;\
}
#endif
#define ALIGN_MEM(out_mem){\
	unsigned long tmp = (unsigned long)out_mem;\
	if(tmp & ~(CACHE_LINE - 1)){\
	tmp += (CACHE_LINE - 1);\
	tmp = (tmp & (~(CACHE_LINE - 1)));\
	}\
	out_mem = (VO_U8*)tmp;\
}


VO_U32 alpha_value = 0;

static VOCCRETURNCODE ccResizeCropFrame(struct CC_HND *cc_hnd, ClrConvData *input_conv_data)
{
    ClrConvData *conv_data = input_conv_data;
    VO_U32 rc = VO_ERR_NONE;

    rc = ((CLRCONV_FUNC_PTR)cc_hnd->ClrConvFun)(conv_data, cc_hnd->x_resize_tab, cc_hnd->y_resize_tab, cc_hnd);

    return VO_ERR_NONE;
}

static VOCCRETURNCODE NV12ToUVPlanar(ClrConvData *conv_data, VO_U8 *dst_u, VO_U8 *dst_v)
{
    VO_S32 i, j;
    const VO_S32 in_stride = conv_data->nInStride;
    const VO_S32 in_height = (conv_data->nInHeight >> 1);
    const VO_S32 in_width = conv_data->nInWidth;

    for(i = 0; i < in_height; i++)
    {
        VO_U8 *uv_buf = (VO_U8 *)(conv_data->pInBuf[1] + i * in_stride);
        VO_U8 *out_u = (VO_U8 *)(dst_u + (i * in_stride >> 1));
        VO_U8 *out_v = (VO_U8 *)(dst_v + (i * in_stride >> 1));

        for(j = 0; j < in_width; j += 2)
        {
            *(out_u++) = *(uv_buf++);
            *(out_v++) = *(uv_buf++);
        }
    }
    return VO_ERR_NONE;
}

static VOCCRETURNCODE ccConvAFrame(struct CC_HND *cc_hnd, ClrConvData *input_conv_data)
{
    ClrConvData conv_nv12, *conv_data = input_conv_data;

    VO_U32 isPlanar, is420Planar;
    VO_U32 rc = VO_ERR_NONE;


    if(conv_data->nInType == VO_COLOR_YUV_420_PACK || conv_data->nInType == VO_COLOR_YUV_420_PACK_2)
    {
        if(conv_data->nInType == VO_COLOR_YUV_420_PACK)
            NV12ToUVPlanar(conv_data, cc_hnd->yuv_nv12[0], cc_hnd->yuv_nv12[1]);
        else
            NV12ToUVPlanar(conv_data, cc_hnd->yuv_nv12[1], cc_hnd->yuv_nv12[0]);
        conv_nv12 = *input_conv_data;
        conv_nv12.pInBuf[1] = cc_hnd->yuv_nv12[0];
        conv_nv12.pInBuf[2] = cc_hnd->yuv_nv12[1];
        conv_nv12.nInType = VO_COLOR_YUV_PLANAR420;
        conv_nv12.nInUVStride = conv_nv12.nInStride / 2;
        conv_data = &conv_nv12;
    }

    isPlanar = TP_IS_PLANAR(conv_data->nInType);
    is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

    if(conv_data->nInType == VO_COLOR_YVU_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR422_12)
    {
        SWAP_SRC(conv_data->pInBuf[1], conv_data->pInBuf[2]); //TBD Don't modify the input structure*/
    }

#ifdef COLCONVENABLE
    if(isPlanar)
    {
        // static VO_S32 nInWidth, nInHeight, nOutWidth, nOutHeight;
        const VO_U32 nIsResize = conv_data->nIsResize;

        if(nIsResize)
        {
            if ((VO_COLOR_RGB888_PLANAR != conv_data->nInType) && (!is420Planar) && (VO_COLOR_YUV_PLANAR422_12 != conv_data->nInType) &&
                    (VO_COLOR_YVU_PLANAR422_12 != conv_data->nInType))
            {
                if(conv_data->nRotationType == ROTATION_90L)
                {
                    YUVPlanarRt90L(conv_data->pInBuf[0], cc_hnd->scale_buf[0], conv_data->nInWidth, conv_data->nInHeight , conv_data->nInStride, conv_data->nInHeight);
                    YUVPlanarRt90L(conv_data->pInBuf[1], cc_hnd->scale_buf[1], conv_data->nInWidth >> 1, (conv_data->nInHeight) >> 1, conv_data->nInStride >> 1, conv_data->nInHeight >> 1);
                    YUVPlanarRt90L(conv_data->pInBuf[2], cc_hnd->scale_buf[2], conv_data->nInWidth >> 1, (conv_data->nInHeight) >> 1, conv_data->nInStride >> 1, conv_data->nInHeight >> 1);
                    cc_hnd->conv_internal.pInBuf[0] = cc_hnd->scale_buf[0];
                    cc_hnd->conv_internal.pInBuf[1] = cc_hnd->scale_buf[1];
                    cc_hnd->conv_internal.pInBuf[2] = cc_hnd->scale_buf[2];
                    cc_hnd->conv_internal.pOutBuf[0] = conv_data->pOutBuf[0];
                    cc_hnd->conv_internal.pOutBuf[1] = conv_data->pOutBuf[1];
                    cc_hnd->conv_internal.pOutBuf[2] = conv_data->pOutBuf[2];
                    cc_hnd->conv_internal.nOutStride = conv_data->nOutStride;
                    conv_data = &(cc_hnd->conv_internal);
                }
                else if(conv_data->nRotationType == ROTATION_90R)
                {
                    YUVPlanarRt90R(conv_data->pInBuf[0], cc_hnd->scale_buf[0], conv_data->nInWidth, conv_data->nInHeight, conv_data->nInStride, conv_data->nInHeight);
                    YUVPlanarRt90R(conv_data->pInBuf[1], cc_hnd->scale_buf[1], conv_data->nInWidth >> 1, (conv_data->nInHeight) >> 1, conv_data->nInStride >> 1, conv_data->nInHeight >> 1);
                    YUVPlanarRt90R(conv_data->pInBuf[2], cc_hnd->scale_buf[2], conv_data->nInWidth >> 1, (conv_data->nInHeight) >> 1, conv_data->nInStride >> 1, conv_data->nInHeight >> 1);
                    cc_hnd->conv_internal.pInBuf[0] = cc_hnd->scale_buf[0];
                    cc_hnd->conv_internal.pInBuf[1] = cc_hnd->scale_buf[1];
                    cc_hnd->conv_internal.pInBuf[2] = cc_hnd->scale_buf[2];
                    cc_hnd->conv_internal.pOutBuf[0] = conv_data->pOutBuf[0];
                    cc_hnd->conv_internal.pOutBuf[1] = conv_data->pOutBuf[1];
                    cc_hnd->conv_internal.pOutBuf[2] = conv_data->pOutBuf[2];
                    cc_hnd->conv_internal.nOutStride = conv_data->nOutStride;
                    conv_data = &(cc_hnd->conv_internal);
                }
            }
        }
    }
#endif // COLCONVENABLE

    rc = ((CLRCONV_FUNC_PTR)cc_hnd->ClrConvFun)(conv_data, cc_hnd->x_resize_tab, cc_hnd->y_resize_tab, cc_hnd);

    return rc;
}

static VOCCRETURNCODE ccGetFunPtr(struct CC_HND *cc_hnd, ClrConvData *input_conv_data)
{
    ClrConvData *conv_data = input_conv_data;
    VO_S32  doconversion = 1;
    VO_U32 rc = VO_ERR_NONE;

    if ((VO_COLOR_YUV_PLANAR420 == conv_data->nInType) && (VO_COLOR_YUV_420_PACK == conv_data->nOutType || VO_COLOR_YUV_420_PACK_2 == conv_data->nOutType))
    {
        doconversion = 0;
    }

    if((conv_data->nInType == conv_data->nOutType) || !doconversion)
    {
        //	static VO_S32 nInWidth, nInHeight, nOutWidth, nOutHeight;
        const VO_U32 nIsResize = conv_data->nIsResize;

        const VO_U32 isRt90 = (conv_data->nRotationType == ROTATION_90L || conv_data->nRotationType == ROTATION_90R) ;
        const VO_S32 isAntiAliasing = (conv_data->nAntiAliasLevel != 0);

        if(((VO_U32)(conv_data->nIsResize) > 1) || ((VO_U32)(conv_data->nRotationType) > 7) ||
                ((VO_U32)(conv_data->nAntiAliasLevel) > 2))
            return VO_ERR_INVALID_ARG;

        if(conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420)
        {
            VO_U32 isRt180 = 0;

            if(ROTATION_180 == conv_data->nRotationType)
            {
                isRt180 = 2;
            }

#ifdef COLCONVENABLE
            if(VO_COLOR_YUV_420_PACK == conv_data->nOutType)
            {
                if((0 != (conv_data->nOutWidth % 8)) || ( 0 != (conv_data->nOutHeight % 8)) || (0 != (conv_data->nOutStride % 8)))
                {
                    return VO_ERR_INVALID_ARG;
                }
                cc_hnd->ClrConvFun = (VO_HANDLE)YUV420PLANARTOYUV420PACK_RESIZE[nIsResize + isAntiAliasing][isRt90 + isRt180];
            }
            else if (VO_COLOR_YUV_420_PACK_2 == conv_data->nOutType)
            {
                if((0 != (conv_data->nOutWidth % 8)) || ( 0 != (conv_data->nOutHeight % 8)) || (0 != (conv_data->nOutStride % 8)))
                {
                    return VO_ERR_INVALID_ARG;
                }
                cc_hnd->ClrConvFun = (VO_HANDLE)YUV420PLANARTOYUV420PACK2_RESIZE[nIsResize + isAntiAliasing][isRt90 + isRt180];
            }
            else
            {
#endif
                if((0 != (conv_data->nOutWidth % 8)) || ( 0 != (conv_data->nOutHeight % 8)) || (0 != (conv_data->nOutStride % 8)))
                {
                    return VO_ERR_INVALID_ARG;
                }

                cc_hnd->ClrConvFun = (VO_HANDLE)YUV420PLANAR_RESIZE[nIsResize + isAntiAliasing][isRt90 + isRt180];
#ifdef COLCONVENABLE
            }
#endif



        }
        else if(conv_data->nInType == VO_COLOR_RGB565_PACKED || conv_data->nInType == VO_COLOR_RGB555_PACKED)
        {

            if((0 != (conv_data->nOutWidth % 2)) || (0 != (conv_data->nOutHeight % 2)) ||
                    (0 != (conv_data->nOutStride % 2)) || (ROTATION_180 == conv_data->nRotationType))
                return VO_ERR_INVALID_ARG;

            cc_hnd->ClrConvFun = (VO_HANDLE)RGB16_RESIZE[nIsResize + isAntiAliasing][isRt90];
        }
#ifdef COLCONVENABLE
        else if (conv_data->nInType == VO_COLOR_YUYV422_PACKED || \
                 conv_data->nInType == VO_COLOR_YVYU422_PACKED || \
                 conv_data->nInType == VO_COLOR_UYVY422_PACKED || \
                 conv_data->nInType == VO_COLOR_VYUY422_PACKED || \
                 conv_data->nInType == VO_COLOR_YUYV422_PACKED_2 || \
                 conv_data->nInType == VO_COLOR_YVYU422_PACKED_2 || \
                 conv_data->nInType == VO_COLOR_UYVY422_PACKED_2 || \
                 conv_data->nInType == VO_COLOR_VYUY422_PACKED_2)
        {

            if((0 != (conv_data->nOutWidth % 4)) || (0 != (conv_data->nOutHeight % 4)) || (0 != (conv_data->nOutStride % 4)) ||
                    (ROTATION_180 == conv_data->nRotationType))
            {
                return VO_ERR_INVALID_ARG;
            }

            cc_hnd->ClrConvFun = (VO_HANDLE)YUV422PACK_RESIZE[nIsResize][isRt90];
        }
        else if(conv_data->nInType == VO_COLOR_YUV_420_PACK || conv_data->nInType == VO_COLOR_YUV_420_PACK_2)
        {

            if((0 != (conv_data->nOutWidth % 4)) || (0 != (conv_data->nOutHeight % 4)) || (0 != (conv_data->nOutStride % 4)) || (ROTATION_180 == conv_data->nRotationType))
            {
                return VO_ERR_INVALID_ARG;
            }

            cc_hnd->ClrConvFun = (VO_HANDLE)YUV420PACKED_RESIZE[nIsResize][isRt90];
        }
#endif // COLCONVENABLE	
        else
        {
            return VO_ERR_CC_UNSUPORT_INTYPE;
        }
    }
    else
    {
        ClrConvData conv_nv12;
        //	static VO_S32 nInWidth, nInHeight, nOutWidth, nOutHeight;
        const VO_U32 nIsResize = conv_data->nIsResize;
        VO_U32 isPlanar, is420Planar;

        if(((VO_U32)(conv_data->nIsResize) > 1) || ((VO_U32)(conv_data->nRotationType) > 7) ||
                ((VO_U32)(conv_data->nAntiAliasLevel) > 3))
            return VO_ERR_INVALID_ARG;

        if(conv_data->nInType == VO_COLOR_YUV_420_PACK || conv_data->nInType == VO_COLOR_YUV_420_PACK_2)
        {
            conv_nv12 = *input_conv_data;
            conv_nv12.nInType = VO_COLOR_YUV_PLANAR420;
            conv_data = &conv_nv12;
        }

        isPlanar = TP_IS_PLANAR(conv_data->nInType);
        is420Planar = (conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_YVU_PLANAR420);

        if(isPlanar)
        {

            if(conv_data->nOutType == VO_COLOR_RGB888_PACKED)
            {

                if((0 != conv_data->nOutWidth % 2) || (0 != conv_data->nOutHeight % 2) || nIsResize ||
                        (0 != conv_data->nOutStride % 2) || (ROTATION_DISABLE != conv_data->nRotationType))
                {
                    return VO_ERR_INVALID_ARG;
                }

                cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanarToRGB24[nIsResize];
            }
            else if((conv_data->nInType == VO_COLOR_YUV_PLANAR420) && (conv_data->nOutType == VO_COLOR_YUYV422_PACKED))
            {
                cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanarToYUYV[nIsResize][0];
            }
            else if((conv_data->nInType == VO_COLOR_YUV_PLANAR420) && (conv_data->nOutType == VO_COLOR_UYVY422_PACKED))
            {
                cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanarToUYVY[nIsResize][0];
            }
            else if(conv_data->nOutType == VO_COLOR_RGB565_PACKED || conv_data->nOutType == VO_COLOR_RGB555_PACKED)
            {
                if(conv_data->nRotationType == FLIP_Y) // TBD merge flip to noresize
                {
                    cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanarToRGB16_normal_flip;
                }
                else
                {
                    if((VO_COLOR_YUV_PLANAR444 == conv_data->nInType) || (VO_COLOR_YUV_PLANAR422_21 == conv_data->nInType) ||
                            (VO_COLOR_YUV_PLANAR411 == conv_data->nInType) || (VO_COLOR_YUV_PLANAR411V == conv_data->nInType))
                    {

                        if ((ROTATION_DISABLE != conv_data->nRotationType) || ( 0 != (conv_data->nOutHeight % 2)) || (!nIsResize))
                        {
                            return VO_ERR_INVALID_ARG;
                        }
                        else if((nIsResize) && ((0 != (conv_data->nOutWidth % 2)) || ( 0 != (conv_data->nOutHeight % 2))))
                        {
                            return VO_ERR_INVALID_ARG;
                        }
                    }

                    if((!nIsResize) && (ROTATION_DISABLE == conv_data->nRotationType))
                    {

#ifdef COLCONVENABLE
                        if(VO_COLOR_YUV_PLANAR444 == conv_data->nInType)
                        {
                            cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanar444ToRGB16_noresize_noRotation;
                        }
                        else if(VO_COLOR_YUV_PLANAR422_21 == conv_data->nInType)
                        {
                            cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanar422_21ToRGB16_noresize_noRotation;
                        }
                        else if(VO_COLOR_YUV_PLANAR411 == conv_data->nInType)
                        {
                            cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanar411ToRGB16_noresize_noRotation;
                        }
                        else if(VO_COLOR_YUV_PLANAR411V == conv_data->nInType)
                        {
                            cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanar411VToRGB16_noresize_noRotation;
                        }
                        else if(VO_COLOR_RGB888_PLANAR == conv_data->nInType)
                        {
                            cc_hnd->ClrConvFun = (VO_HANDLE)RGB888PlanarToRGB16_noresize_noRotation;

                        }
                        else
#endif // COLCONVENABLE						
                        {
                            if((0 != (conv_data->nOutWidth % 2)) || ( 0 != (conv_data->nOutHeight % 2)))
                            {
                                return VO_ERR_INVALID_ARG;
                            }

                            cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanarToRGB16_noresize_noRotation;
                        }

                    }
                    else if(nIsResize)
                    {
                        VO_S32 rotation_type = 0;
                        const VO_S32 isAntiAliasing = (conv_data->nAntiAliasLevel != 0);

                        if (ROTATION_90L == conv_data->nRotationType)
                        {
                            rotation_type = 1 + (!isAntiAliasing);
                        }
                        else if (ROTATION_90R == conv_data->nRotationType)
                        {
                            rotation_type = 2 + (!isAntiAliasing);
                        }
                        else if (ROTATION_180 == conv_data->nRotationType)
                        {
                            rotation_type = 3 + (!isAntiAliasing);
                        }
#ifdef COLCONVENABLE
                        if(VO_COLOR_YUV_PLANAR444 == conv_data->nInType)
                        {
                            cc_hnd->ClrConvFun = (VO_HANDLE)YUV444PlanarToRGB16ResizeNoRotation[isAntiAliasing];
                        }
                        else if(VO_COLOR_YUV_PLANAR422_21 == conv_data->nInType)
                        {
                            cc_hnd->ClrConvFun = (VO_HANDLE)YUV422_21PlanarToRGB16ResizeNoRotation[isAntiAliasing];
                        }
                        else if(VO_COLOR_YUV_PLANAR411 == conv_data->nInType)
                        {
                            cc_hnd->ClrConvFun = (VO_HANDLE)YUV411PlanarToRGB16ResizeNoRotation[isAntiAliasing];
                        }
                        else if(VO_COLOR_YUV_PLANAR411V == conv_data->nInType)
                        {
                            cc_hnd->ClrConvFun = (VO_HANDLE)YUV411VPlanarToRGB16ResizeNoRotation[isAntiAliasing];
                        }
                        else if(VO_COLOR_RGB888_PLANAR == conv_data->nInType)
                        {

                            if((0 != (conv_data->nOutWidth % 2)) || ( 0 != (conv_data->nOutHeight % 2)) || (0 != (conv_data->nOutStride % 2)))
                            {
                                return VO_ERR_INVALID_ARG;
                            }

#ifdef VOARMV7
                            if(0 != (conv_data->nOutStride % 8))
                            {
                                return VO_ERR_INVALID_ARG;
                            }
#endif
                            cc_hnd->ClrConvFun = (VO_HANDLE)RGB888PlanarToRGB16ResizeNoRotation[isAntiAliasing + rotation_type];
                        }
                        else
#endif // COLCONVENABLE
                        {
                            VO_S32 offset     = 0;
                            VO_S32 rotation_number = 0;
                            VO_S32 in_width   = conv_data->nInWidth;
                            VO_S32 in_height  = conv_data->nInHeight;
                            VO_S32 out_width  = conv_data->nOutWidth;
                            VO_S32 out_height = conv_data->nOutHeight;

                            if((0 != (conv_data->nOutWidth % 2)) || ( 0 != (conv_data->nOutHeight % 2)) || (0 != (conv_data->nOutStride % 2)))
                                return VO_ERR_INVALID_ARG;
#ifdef VOARMV7
                            if(0 != (conv_data->nOutStride % 8))
                            {
                                return VO_ERR_INVALID_ARG;
                            }
#endif

#ifndef VOARMV7
                            if((((in_width >> 1) == out_width) && ((in_height >> 1) == out_height))
                                    || (((in_width >> 2) == out_width) && ((in_height >> 2) == out_height))
                                    || (((in_width >> 3) == out_width) && ((in_height >> 3) == out_height))
                                    || (((in_width >> 4) == out_width) && ((in_height >> 4) == out_height))
                                    || (((in_width >> 5) == out_width) && ((in_height >> 5) == out_height))
                                    || (((in_width >> 6) == out_width) && ((in_height >> 6) == out_height))
                                    || (((in_width >> 7) == out_width) && ((in_height >> 7) == out_height))
                                    || (((in_width >> 8) == out_width) && ((in_height >> 8) == out_height))
                                    || (((in_width >> 9) == out_width) && ((in_height >> 9) == out_height))
                                    || (((in_width >> 10) == out_width) && ((in_height >> 10) == out_height))
                              )
                                offset = 1;
                            else if(((in_width * 3) == (out_width * 4)) && ((in_height * 3) == (out_height * 4)))
                                offset = 2;
                            else if(((in_width * 3) == (out_width * 2)) && ((in_height * 3) == (out_height * 2)))
                                offset = 3;
                            else if(((in_width << 1) == out_width) && ((in_height << 1) == out_height))
                                offset = 4;
                            else
#endif
                                offset = 0;

                            if (ROTATION_90L == conv_data->nRotationType)
                            {
                                rotation_number = 5 + offset;
                            }
                            else if (ROTATION_90R == conv_data->nRotationType)
                            {
                                rotation_number = 10 + offset;
                            }
                            else if (ROTATION_180 == conv_data->nRotationType)
                            {
                                rotation_number = 15;
                            }
                            else if (isAntiAliasing)
                            {
                                rotation_number = 16;
                            }
                            else
                            {
                                rotation_number = offset;
                            }

                            if ((320 == in_width) && (424 == out_width) && (240 == in_height)  && (320 == out_height) && (ROTATION_DISABLE == conv_data->nRotationType))
                                rotation_number = 17;

                            cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanarToRGB16ResizeRotation[rotation_number];
                        }

                    }
                    else
                    {
#ifdef COLCONVENABLE
                        if(VO_COLOR_RGB888_PLANAR == conv_data->nInType)
                        {

                            if((0 != (conv_data->nOutWidth % 2)) || ( 0 != (conv_data->nOutHeight % 2)) || (0 != (conv_data->nOutStride % 2)))
                                return VO_ERR_INVALID_ARG;
#ifdef VOARMV7
                            if(0 != (conv_data->nOutStride % 8))
                            {
                                return VO_ERR_INVALID_ARG;
                            }
#endif
                            cc_hnd->ClrConvFun = (VO_HANDLE)RGB888PlanarToRGB16[nIsResize];
                        }
                        else
#endif //COLCONVENABLE
                        {
                            if((0 != (conv_data->nOutWidth % 2)) || ( 0 != (conv_data->nOutHeight % 2)) || (0 != (conv_data->nOutStride % 2)))
                                return VO_ERR_INVALID_ARG;
#ifdef VOARMV7
                            if(0 != (conv_data->nOutStride % 8))
                            {
                                return VO_ERR_INVALID_ARG;
                            }
#endif
                            cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanarToRGB16[nIsResize];
                        }
                    }

                }
            }
            else
            {
#ifdef COLCONVENABLE
                if(conv_data->nOutType == VO_COLOR_YUV_PLANAR420 || conv_data->nOutType == VO_COLOR_YVU_PLANAR420)
                {
                    cc_hnd->ClrConvFun = (VO_HANDLE)YUV422PlanarToYUV420Planar;
                }
                else
#endif
                    if (conv_data->nOutType == VO_COLOR_RGB32_PACKED)
                    {

                        if((0 != (conv_data->nOutWidth % 2)) || ( 0 != (conv_data->nOutHeight % 2)) || (0 != (conv_data->nOutStride % 2)))
                            return VO_ERR_INVALID_ARG;

                        if(nIsResize)
                        {
                            VO_S32 offset     = 0;
                            VO_S32 rotation_number = 0;
#ifndef VOARMV7
                            VO_S32 in_width   = conv_data->nInWidth;
                            VO_S32 in_height  = conv_data->nInHeight;
                            VO_S32 out_width  = conv_data->nOutWidth;
                            VO_S32 out_height = conv_data->nOutHeight;

                            if((((in_width >> 1) == out_width) && ((in_height >> 1) == out_height))
                                    || (((in_width >> 2) == out_width) && ((in_height >> 2) == out_height))
                                    || (((in_width >> 3) == out_width) && ((in_height >> 3) == out_height))
                                    || (((in_width >> 4) == out_width) && ((in_height >> 4) == out_height))
                                    || (((in_width >> 5) == out_width) && ((in_height >> 5) == out_height))
                                    || (((in_width >> 6) == out_width) && ((in_height >> 6) == out_height))
                                    || (((in_width >> 7) == out_width) && ((in_height >> 7) == out_height))
                                    || (((in_width >> 8) == out_width) && ((in_height >> 8) == out_height))
                                    || (((in_width >> 9) == out_width) && ((in_height >> 9) == out_height))
                                    || (((in_width >> 10) == out_width) && ((in_height >> 10) == out_height))
                              )
                                offset = 1;
                            else
#endif
                                offset = 0;
                            if (ROTATION_90L == conv_data->nRotationType)
                            {
                                rotation_number = 2 + offset;
                            }
                            else if (ROTATION_90R == conv_data->nRotationType)
                            {
                                rotation_number = 4 + offset;
                            }
                            else if (ROTATION_180 == conv_data->nRotationType)
                            {
                                rotation_number = 6;
                            }
                            else
                            {
                                rotation_number = offset;
                            }

                            cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanarToRGB32ResizeRotation[rotation_number];
#ifdef  VOX86
                            cc_hnd->ClrConvFunIndex = rotation_number + RGB32ResizeNoRot;
#endif
                        }
                        else
                        {
                            cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanarToRGB32_noresize;
#ifdef  VOX86
                            if (ROTATION_DISABLE == conv_data->nRotationType)
							   cc_hnd->ClrConvFunIndex =  RGB32NoResizeNoRot;
							else
							   cc_hnd->ClrConvFunIndex =  RGB32NoResizeRot;
#endif							
                        }


                    }
                    else if(conv_data->nOutType == VO_COLOR_ARGB32_PACKED)
                    {
                        if((0 != (conv_data->nOutWidth % 2)) || ( 0 != (conv_data->nOutHeight % 2)) || (0 != (conv_data->nOutStride % 2)))
                            return VO_ERR_INVALID_ARG;

                        if(nIsResize)
                        {
                            VO_S32 offset     = 0;
                            VO_S32 rotation_number = 0;
#ifndef VOARMV7
                            VO_S32 in_width   = conv_data->nInWidth;
                            VO_S32 in_height  = conv_data->nInHeight;
                            VO_S32 out_width  = conv_data->nOutWidth;
                            VO_S32 out_height = conv_data->nOutHeight;

                            if((((in_width >> 1) == out_width) && ((in_height >> 1) == out_height))
                                    || (((in_width >> 2) == out_width) && ((in_height >> 2) == out_height))
                                    || (((in_width >> 3) == out_width) && ((in_height >> 3) == out_height))
                                    || (((in_width >> 4) == out_width) && ((in_height >> 4) == out_height))
                                    || (((in_width >> 5) == out_width) && ((in_height >> 5) == out_height))
                                    || (((in_width >> 6) == out_width) && ((in_height >> 6) == out_height))
                                    || (((in_width >> 7) == out_width) && ((in_height >> 7) == out_height))
                                    || (((in_width >> 8) == out_width) && ((in_height >> 8) == out_height))
                                    || (((in_width >> 9) == out_width) && ((in_height >> 9) == out_height))
                                    || (((in_width >> 10) == out_width) && ((in_height >> 10) == out_height))
                              )
                                offset = 1;
                            else
#endif
                                offset = 0;
                            if (ROTATION_90L == conv_data->nRotationType)
                            {
                                rotation_number = 2 + offset;
                            }
                            else if (ROTATION_90R == conv_data->nRotationType)
                            {
                                rotation_number = 4 + offset;
                            }
                            else if (ROTATION_180 == conv_data->nRotationType)
                            {
                                rotation_number = 6;
                            }
                            else
                            {
                                rotation_number = offset;
                            }
#if DEBUG_DLL
                            {
							  FILE *f;
							  VO_S8 buff[128];
							  f = fopen("D:/cc.log", "a+");
							  sprintf(buff, "rotation_number %d \n",rotation_number);
							  fputs(buff, f);
							  fclose(f);
                            }
#endif	
							

                            cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanarToARGB32ResizeRotation[rotation_number];
#ifdef VOX86
                            cc_hnd->ClrConvFunIndex = rotation_number + ARGB32ResizeNoRot;
#endif
                        }
                        else
                        {
                            cc_hnd->ClrConvFun = (VO_HANDLE)YUVPlanarToARGB32_noresize;
#ifdef  VOX86
							if (ROTATION_DISABLE == conv_data->nRotationType)
							   cc_hnd->ClrConvFunIndex =  ARGB32NoResizeNoRot;
							else
							   cc_hnd->ClrConvFunIndex =  ARGB32NoResizeRot;
#endif							
							
                        }
                    }
                    else
                    {
                        return VO_ERR_CC_UNSUPORT_OUTTYPE;
                    }
            }

        }
#ifdef COLCONVENABLE
        else if (conv_data->nInType == VO_COLOR_YUYV422_PACKED || conv_data->nInType == VO_COLOR_YUYV422_PACKED_2)
        {
            if(conv_data->nOutType == VO_COLOR_RGB565_PACKED || conv_data->nOutType == VO_COLOR_RGB555_PACKED)
            {

                if ((!nIsResize) &&
                        ((0 != (conv_data->nOutWidth % 16)) || ( 0 != (conv_data->nOutHeight % 16)) || (0 != (conv_data->nOutStride % 16))))
                {
                    return VO_ERR_INVALID_ARG;
                }

                if(nIsResize &&
                        ((0 != (conv_data->nOutWidth % 2)) || (0 != (conv_data->nOutHeight % 2)) || ( ROTATION_180 == conv_data->nRotationType) || (0 != (conv_data->nOutStride % 2))))
                {
                    return VO_ERR_INVALID_ARG;

                }
                cc_hnd->ClrConvFun = (VO_HANDLE)YUV422InterlaceToRGB16[nIsResize];
            }
            else if(conv_data->nOutType == VO_COLOR_RGB888_PACKED)
            {

                if((0 != (conv_data->nOutWidth % 2)) || (0 != (conv_data->nOutStride % 2)) || (ROTATION_180 == conv_data->nRotationType))
                {
                    return VO_ERR_INVALID_ARG;
                }

                cc_hnd->ClrConvFun = (VO_HANDLE)YUV422InterlaceToRGB24[nIsResize];
            }
            else if(conv_data->nOutType == VO_COLOR_YUV_PLANAR420)
            {

                if((0 != conv_data->nOutWidth % 4) || (0 != conv_data->nOutHeight % 2) || nIsResize ||
                        (0 != conv_data->nOutStride % 4) || (ROTATION_DISABLE != conv_data->nRotationType))
                {
                    return VO_ERR_INVALID_ARG;
                }

                cc_hnd->ClrConvFun = (VO_HANDLE)YUV422PackedToYUV420Planar_noresize;
            }
            else
            {
                return VO_ERR_CC_UNSUPORT_OUTTYPE;
            }
        }
        else if (conv_data->nInType == VO_COLOR_YVYU422_PACKED || conv_data->nInType == VO_COLOR_YVYU422_PACKED_2)
        {
            if(conv_data->nOutType == VO_COLOR_RGB565_PACKED || conv_data->nOutType == VO_COLOR_RGB555_PACKED)
            {

                if((!nIsResize) &&
                        ((0 != (conv_data->nOutWidth % 16)) || ( 0 != (conv_data->nOutHeight % 16)) || (0 != (conv_data->nOutStride % 16))))
                {
                    return VO_ERR_INVALID_ARG;
                }

                if(nIsResize &&
                        ((0 != (conv_data->nOutWidth % 2)) || ( ROTATION_180 == conv_data->nRotationType) || (0 != (conv_data->nOutStride % 2))))
                {
                    return VO_ERR_INVALID_ARG;
                }
                cc_hnd->ClrConvFun = (VO_HANDLE)YUV422InterlaceToRGB16[nIsResize];
            }
            else if(conv_data->nOutType == VO_COLOR_RGB888_PACKED)
            {

                if((0 != (conv_data->nOutWidth % 2)) || (0 != (conv_data->nOutStride % 2)) || (ROTATION_180 == conv_data->nRotationType))
                {
                    return VO_ERR_INVALID_ARG;
                }

                cc_hnd->ClrConvFun = (VO_HANDLE)YUV422InterlaceToRGB24[nIsResize];
            }
            else if(conv_data->nOutType == VO_COLOR_YUV_PLANAR420)
            {

                if((0 != conv_data->nOutWidth % 4) || (0 != conv_data->nOutHeight % 2) || nIsResize ||
                        (0 != conv_data->nOutStride % 4) || (ROTATION_DISABLE != conv_data->nRotationType))
                {
                    return VO_ERR_INVALID_ARG;
                }
                cc_hnd->ClrConvFun = (VO_HANDLE)YUV422PackedToYUV420Planar_noresize;
            }
            else
            {
                return VO_ERR_CC_UNSUPORT_OUTTYPE;
            }
        }
        else if (conv_data->nInType == VO_COLOR_UYVY422_PACKED || conv_data->nInType == VO_COLOR_UYVY422_PACKED_2)
        {
            if(conv_data->nOutType == VO_COLOR_RGB565_PACKED || conv_data->nOutType == VO_COLOR_RGB555_PACKED)
            {

                if((!nIsResize) && ((0 != (conv_data->nOutWidth % 16)) || ( 0 != (conv_data->nOutHeight % 16)) || (0 != (conv_data->nOutStride % 16))))
                {
                    return VO_ERR_INVALID_ARG;
                }

                if(nIsResize && ((0 != (conv_data->nOutWidth % 2)) || ( ROTATION_180 == conv_data->nRotationType) || (0 != (conv_data->nOutStride % 2))))
                {
                    return VO_ERR_INVALID_ARG;
                }
                cc_hnd->ClrConvFun = (VO_HANDLE)YUV422InterlaceToRGB16[nIsResize];
            }
            else if(conv_data->nOutType == VO_COLOR_RGB888_PACKED)
            {

                if((0 != (conv_data->nOutWidth % 2)) || (0 != (conv_data->nOutStride % 2)) || (ROTATION_180 == conv_data->nRotationType))
                {
                    return VO_ERR_INVALID_ARG;
                }

                cc_hnd->ClrConvFun = (VO_HANDLE)YUV422InterlaceToRGB24[nIsResize];
            }
            else if(conv_data->nOutType == VO_COLOR_YUV_PLANAR420)
            {

                if((0 != conv_data->nOutWidth % 4) || (0 != conv_data->nOutHeight % 2) || nIsResize ||
                        (0 != conv_data->nOutStride % 4) || (ROTATION_DISABLE != conv_data->nRotationType))
                {
                    return VO_ERR_INVALID_ARG;
                }
                cc_hnd->ClrConvFun = (VO_HANDLE)YUV422PackedToYUV420Planar_noresize;
            }
            else
            {
                return VO_ERR_CC_UNSUPORT_OUTTYPE;
            }
        }
        else if (conv_data->nInType == VO_COLOR_VYUY422_PACKED || conv_data->nInType == VO_COLOR_VYUY422_PACKED_2)
        {
            if(conv_data->nOutType == VO_COLOR_RGB565_PACKED || conv_data->nOutType == VO_COLOR_RGB555_PACKED)
            {

                if((!nIsResize) && ((0 != (conv_data->nOutWidth % 16)) || ( 0 != (conv_data->nOutHeight % 16)) || (0 != (conv_data->nOutStride % 16))))
                {
                    return VO_ERR_INVALID_ARG;
                }

                if(nIsResize && ((0 != (conv_data->nOutWidth % 2)) || ( ROTATION_180 == conv_data->nRotationType) || (0 != (conv_data->nOutStride % 2))))
                {
                    return VO_ERR_INVALID_ARG;
                }
                cc_hnd->ClrConvFun = (VO_HANDLE)YUV422InterlaceToRGB16[nIsResize];
            }
            else if(conv_data->nOutType == VO_COLOR_RGB888_PACKED)
            {

                if((0 != (conv_data->nOutWidth % 2)) || (0 != (conv_data->nOutStride % 2)) || (ROTATION_180 == conv_data->nRotationType))
                {
                    return VO_ERR_INVALID_ARG;
                }

                cc_hnd->ClrConvFun = (VO_HANDLE)YUV422InterlaceToRGB24[nIsResize];
            }
            else if(conv_data->nOutType == VO_COLOR_YUV_PLANAR420)
            {

                if((0 != conv_data->nOutWidth % 4) || (0 != conv_data->nOutHeight % 2) || nIsResize ||
                        (0 != conv_data->nOutStride % 4) || (ROTATION_DISABLE != conv_data->nRotationType))
                {
                    return VO_ERR_INVALID_ARG;
                }

                cc_hnd->ClrConvFun = (VO_HANDLE)YUV422PackedToYUV420Planar_noresize;
            }
            else
            {
                return VO_ERR_CC_UNSUPORT_OUTTYPE;
            }
        }
#endif // COLCONVENABLE
        else if (conv_data->nInType == VO_COLOR_RGB888_PACKED)
        {
            VO_S32 nRotationType = 0;
            if (ROTATION_DISABLE == conv_data->nRotationType)
            {
                nRotationType = 0;
            }
            else if (ROTATION_90L == conv_data->nRotationType)
            {
                nRotationType = 1;
            }
            else if (ROTATION_90R == conv_data->nRotationType)
            {
                nRotationType = 2;
            }

            if(conv_data->nOutType == VO_COLOR_RGB565_PACKED)
            {

                if((!nIsResize) && ((0 != (conv_data->nOutWidth % 4)) || ( 0 != (conv_data->nOutHeight % 4)) || (0 != (conv_data->nOutStride % 4))))
                {
                    return VO_ERR_INVALID_ARG;
                }

                if(nIsResize &&
                        ((0 != (conv_data->nOutWidth % 2)) || ( ROTATION_180 == conv_data->nRotationType) || ( 0 != (conv_data->nOutHeight % 2)) ||
                         (0 != (conv_data->nOutStride % 2))))
                {
                    return VO_ERR_INVALID_ARG;
                }

                cc_hnd->ClrConvFun = (VO_HANDLE)RGB24TORGB565[nIsResize*3 + nRotationType];
            }

        }
        else
        {
            return VO_ERR_CC_UNSUPORT_INTYPE;
        }

    }

    return rc;

}

#define HIGH_X_OF (float)1.6
#define HIGH_Y_OF (float)1.1

#define LOW_X_OF (float)1.4
#define LOW_Y_OF (float)0.9

float anti_alias[2][2] =
{
    {LOW_X_OF, LOW_Y_OF},
    {HIGH_X_OF, HIGH_Y_OF}
};

#ifdef VOARMV7
static void init_scale_VOARMV7_width2(float x_scale, VO_S32 out_width, VO_S32 *scale_tab, VO_S32 in_width)
{
    VO_S32 i, j, *mb_start_pos, len, tmp, x_par;
    float out_pos;
    short aa0, aa1, aa2, aa3, bb0, bb1, bb2, bb3;
    VO_S32 pos0, pos1, pos2, pos3;

    i = 1;
    len = 0;
    mb_start_pos = scale_tab;
    scale_tab++;
    in_width--;

    for(j = 0; j < out_width; )
    {
        //0
        {
            out_pos = x_scale * j;
            tmp = (int)out_pos;
            x_par = (int)((out_pos - tmp) * (1 << 10)); //1024
            len ++;
            if(!((j + 1) % 8))
            {

                if(j & 1) // finish current mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    pos0 = tmp; // out_pos
                    aa0 = x_par; // x
                    bb0 = ((1 << 10) - x_par); // y
                    //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
                }
                else  // start next mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    len--;
                }
            }
            else
            {
                if(tmp >= in_width)
                    tmp = in_width - 1;

                pos0 = tmp; // out_pos
                aa0 = x_par; // x
                bb0 = ((1 << 10) - x_par); // y
                //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
            }
        }
        j++;
        //1
        {
            out_pos = x_scale * j;
            tmp = (int)out_pos;
            x_par = (int)((out_pos - tmp) * (1 << 10)); //1024
            len ++;
            if(!((j + 1) % 8))
            {

                if(j & 1) // finish current mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    pos1 = tmp; // out_pos
                    aa1 = x_par; // x
                    bb1 = ((1 << 10) - x_par); // y
                    //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
                }
                else  // start next mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    len--;
                }
            }
            else
            {
                if(tmp >= in_width)
                    tmp = in_width - 1;

                pos1 = tmp; // out_pos
                aa1 = x_par; // x
                bb1 = ((1 << 10) - x_par); // y
                //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
            }
        }
        j++;
        //2
        {
            out_pos = x_scale * j;
            tmp = (int)out_pos;
            x_par = (int)((out_pos - tmp) * (1 << 10)); //1024
            len ++;
            if(!((j + 1) % 8))
            {

                if(j & 1) // finish current mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    pos2 = tmp; // out_pos
                    aa2 = x_par; // x
                    bb2 = ((1 << 10) - x_par); // y
                    //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
                }
                else  // start next mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    len--;
                }
            }
            else
            {
                if(tmp >= in_width)
                    tmp = in_width - 1;

                pos2 = tmp; // out_pos
                aa2 = x_par; // x
                bb2 = ((1 << 10) - x_par); // y
                //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
            }
        }
        j++;
        //3
        {
            out_pos = x_scale * j;
            tmp = (int)out_pos;
            x_par = (int)((out_pos - tmp) * (1 << 10)); //1024
            len ++;
            if(!((j + 1) % 8))
            {

                if(j & 1) // finish current mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    pos3 = tmp; // out_pos
                    aa3 = x_par; // x
                    bb3 = ((1 << 10) - x_par); // y
                    //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
                }
                else  // start next mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    len--;
                }
            }
            else
            {
                if(tmp >= in_width)
                    tmp = in_width - 1;

                pos3 = tmp; // out_pos
                aa3 = x_par; // x
                bb3 = ((1 << 10) - x_par); // y
                //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
            }
        }
        j++;
        //	short aa0, aa1, aa2, aa3, bb0, bb1, bb2, bb3;
        //	int pos0, pos1, pos2, pos3;
        *(scale_tab++) = (pos1 << 16) | pos0;
        *(scale_tab++) = (pos3 << 16) | pos2;
        //*(scale_tab++) = pos2;
        //*(scale_tab++) = pos3;
        *(scale_tab++) = ((aa1) << 16) | aa0;
        *(scale_tab++) = ((aa3) << 16) | aa2;
        *(scale_tab++) = ((bb1) << 16) | bb0;
        *(scale_tab++) = ((bb3) << 16) | bb2;
        if(!(j % 8))
        {
            *mb_start_pos = len;
            len = 0;
            mb_start_pos = scale_tab;
            scale_tab ++;
        }
    }
    *mb_start_pos = len;
}

static void init_scale_VOARMV7_height2(float x_scale, VO_S32 out_width, VO_S32 *scale_tab, VO_S32 in_width)
{
    VO_S32 i, j, *mb_start_pos, len, tmp, x_par;
    float out_pos;
    short aa0, aa1, bb0, bb1;
    VO_S32 pos0, pos1;

    i = 1;
    len = 0;
    mb_start_pos = scale_tab;
    scale_tab++;
    in_width--;

    for(j = 0; j < out_width; )
    {
        //0
        {
            out_pos = x_scale * j;
            tmp = (int)out_pos;
            x_par = (int)((out_pos - tmp) * (1 << 10)); //1024
            len ++;
            if(!((j + 1) % 8))
            {

                if(j & 1) // finish current mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    pos0 = tmp; // out_pos
                    aa0 = x_par; // x
                    bb0 = ((1 << 10) - x_par); // y
                    //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
                }
                else  // start next mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    len--;
                }
            }
            else
            {
                if(tmp >= in_width)
                    tmp = in_width - 1;

                pos0 = tmp; // out_pos
                aa0 = x_par; // x
                bb0 = ((1 << 10) - x_par); // y
                //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
            }
        }
        j++;
        //1
        {
            out_pos = x_scale * j;
            tmp = (int)out_pos;
            x_par = (int)((out_pos - tmp) * (1 << 10)); //1024
            len ++;
            if(!((j + 1) % 8))
            {

                if(j & 1) // finish current mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    pos1 = tmp; // out_pos
                    aa1 = x_par; // x
                    bb1 = ((1 << 10) - x_par); // y
                    //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
                }
                else  // start next mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    len--;
                }
            }
            else
            {
                if(tmp >= in_width)
                    tmp = in_width - 1;

                pos1 = tmp; // out_pos
                aa1 = x_par; // x
                bb1 = ((1 << 10) - x_par); // y
                //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
            }
        }
        j++;
        //	short aa0, aa1, bb0, bb1;
        //	int pos0, pos1;
        *(scale_tab++) = aa0;
        *(scale_tab++) = bb0;
        *(scale_tab++) = aa1;
        *(scale_tab++) = bb1;
        *(scale_tab++) = pos0;
        *(scale_tab++) = pos1;
        if((!(j % 8)))
        {
            *mb_start_pos = len;
            len = 0;
            mb_start_pos = scale_tab;
            scale_tab ++;
        }

    }
    *mb_start_pos = len;
}

#endif


#ifdef VOX86
static void init_scale_VOX86_width2(float x_scale, VO_S32 out_width, VO_S32 *scale_tab, VO_S32 in_width)
{
    VO_S32 i, j, *mb_start_pos, len, tmp, x_par;
    float out_pos;
    VO_S32 aa0, bb0;
    VO_S32 pos0;
#if DEBUG_DLL
	FILE *f;
	VO_S8 buff[128];
	f = fopen("D:/cc.log", "a+");
	sprintf(buff, "out_width %d in_width %d\n",out_width, in_width);
	fputs(buff, f);
	fclose(f);
#endif	

    i = 1;
    len = 0;
    mb_start_pos = scale_tab;
    scale_tab++;
    in_width--;


    for(j = 0; j < out_width; )
    {
        //0
        {
            out_pos = x_scale * j;
            tmp = (int)out_pos;
            x_par = (int)((out_pos - tmp) * (1 << 10)); //1024
            len ++;
            if(!((j + 1) % 16))
            {

                if(j & 1) // finish current mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    pos0 = tmp; // out_pos
                    aa0 = x_par; // x
                    bb0 = ((1 << 10) - x_par); // y
                    //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
                }
                else  // start next mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    len--;
                }
            }
            else
            {
                if(tmp >= in_width)
                    tmp = in_width - 1;

                pos0 = tmp; // out_pos
                aa0 = x_par; // x
                bb0 = ((1 << 10) - x_par); // y
                //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
            }
        }
        j++;

        *(scale_tab++) = pos0;
        *(scale_tab++) = aa0;
        *(scale_tab++) = bb0;
        if(!(j % 16))
        {
            *mb_start_pos = len;
            len = 0;
            mb_start_pos = scale_tab;
            scale_tab ++;
        }
    }
    *mb_start_pos = len;
}

static void init_scale_VOX86_height2(float x_scale, VO_S32 out_width, VO_S32 *scale_tab, VO_S32 in_width)
{
    VO_S32 i, j, *mb_start_pos, len, tmp, x_par;
    float out_pos;
    short aa0, aa1, bb0, bb1;
    VO_S32 pos0, pos1;
#if DEBUG_DLL
	FILE *f;
	VO_S8 buff[128];
	f = fopen("D:/cc.log", "a+");
	sprintf(buff, "out_height %d in_height %d\n",out_width, in_width);
	fputs(buff, f);
	fclose(f);
#endif	

    i = 1;
    len = 0;
    mb_start_pos = scale_tab;
    scale_tab++;
    in_width--;

    for(j = 0; j < out_width; )
    {
        //0
        {
            out_pos = x_scale * j;
            tmp = (int)out_pos;
            x_par = (int)((out_pos - tmp) * (1 << 10)); //1024
            len ++;
            if(!((j + 1) % 16))
            {

                if(j & 1) // finish current mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    pos0 = tmp; // out_pos
                    aa0 = x_par; // x
                    bb0 = ((1 << 10) - x_par); // y
                    //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
                }
                else  // start next mb
                {
                    if(tmp >= in_width)
                        tmp = in_width - 1;
                    len--;
                }
            }
            else
            {
                if(tmp >= in_width)
                    tmp = in_width - 1;

                pos0 = tmp; // out_pos
                aa0 = x_par; // x
                bb0 = ((1 << 10) - x_par); // y
                //*(scale_tab++) = (((1<<10) - x_par)<<16)|x_par; // y
            }
        }
        j++;
        //1
        //	short aa0, aa1, bb0, bb1;
        //	int pos0, pos1;
        *(scale_tab++) = pos0;
        *(scale_tab++) = aa0;
        *(scale_tab++) = bb0;
        if((!(j % 16)))
        {
            *mb_start_pos = len;
            len = 0;
            mb_start_pos = scale_tab;
            scale_tab ++;
        }

    }
    *mb_start_pos = len;
}

#endif


static void init_scale(float x_scale, VO_S32 out_width, VO_S32 *scale_tab, VO_S32 in_width)
{
    VO_S32 i, j, *mb_start_pos, len, tmp, x_par;
    float out_pos;

    i = 1;
    len = 0;
    mb_start_pos = scale_tab;
    scale_tab++;
    in_width--;

    for(j = 0; j < out_width; j++)
    {
        out_pos = x_scale * j;

        tmp = (int)out_pos;

        x_par = (int)((out_pos - tmp) * 1024); //1024

        len ++;
        if(tmp >= (i * 16 - 1))
        {

            if(j & 1) // finish current mb
            {
                if(tmp >= in_width)
                    tmp = in_width - 1;
                *(scale_tab++) = tmp; // out_pos
                *(scale_tab++) = x_par; // x
                *(scale_tab++) = ((1 << 10) - x_par); // y
                *mb_start_pos = len;
                //				printf("%d length: %d\n", i, len);

                len = 0;
                mb_start_pos = scale_tab;
                scale_tab ++;
            }
            else  // start next mb
            {
                if(tmp >= in_width)
                    tmp = in_width - 1;
                *mb_start_pos = len - 1;
                //				printf("%d length: %d\n", i, len-1);

                mb_start_pos = scale_tab++;

                *(scale_tab++) = tmp; // out_pos
                *(scale_tab++) = x_par; // x
                *(scale_tab++) = ((1 << 10) - x_par); // y
                len = 1;
            }
            i++;
        }
        else
        {
            if(tmp >= in_width)
                tmp = in_width - 1;
            *(scale_tab++) = tmp; // out_pos
            *(scale_tab++) = x_par; // x
            *(scale_tab++) = ((1 << 10) - x_par); // y
        }
    }
    *mb_start_pos = len;
}


static void init_scale_nearest(VO_S32 x_scale, VO_S32 out_width, VO_S32 *scale_tab)
{
    VO_S32 i;

    for(i = 0; i < out_width; i++)
    {
        scale_tab[i] = (VO_S32)((i * x_scale) >> 10);
    }

    scale_tab[i] = scale_tab[i+1] = scale_tab[i+2] = scale_tab[i+3] = 0xff;

}

static void init_scale_nearest_new(VO_S32 x_scale, VO_S32 out_width, VO_S32 *scale_tab)
{
    VO_S32 i, bak = -1, tmp;

    for(i = 0; i < out_width; i++)
    {
        VO_S32 a;
        tmp = (VO_S32)((i * x_scale) >> 10);
        a = tmp - bak;
        tmp = (tmp << 16);

        if(a != 0)
        {
            tmp = tmp | 0x2; //update Y
            if(a > 1)
            {
                tmp = tmp | 0x1;
            }
            else
            {
                if(!(tmp & 0x10000))
                {
                    tmp = tmp | 0x1;
                }
            }
        }

        scale_tab[i] = tmp;
        bak = (tmp >> 16);
    }

}

static void init_scale_bilinear_new(VO_S32 y_scale, VO_S32 out_height, VO_S32 *scale_tab)
{
    VO_S32 i, bak = -1, tmp;

    for(i = 0; i < out_height;)
    {
        VO_S32 a;
        a = 0;
        bak = (VO_S32)(((i++) * y_scale) >> 10);
        tmp = (VO_S32)(((i++) * y_scale) >> 10);

        while(tmp == bak)
        {
            a++;
            tmp = (VO_S32)(((i++) * y_scale) >> 10);
        }

        if(i >= out_height)
            break;
        if(a != 0)
        {
            *(scale_tab++) = -a;
        }

        a = tmp - bak;
        //		i++;
        if(a > 1)
        {
            *(scale_tab++) = bak;
            *(scale_tab++) = tmp;
        }
        else
        {
            if(!(bak & 0x1))
            {
                *(scale_tab++) = bak | 0x10000;
            }
            else
            {
                *(scale_tab++) = bak;
                *(scale_tab++) = tmp;
            }
        }

        bak = (VO_S32)((i * y_scale) >> 10);
        a = 0;

        while((tmp == bak) && (i < out_height))
        {
            i++;
            a++;
            tmp = (VO_S32)((i * y_scale) >> 10);
        }
        if(i >= out_height)
            break;
        if(a != 0)
        {
            *(scale_tab++) = -a;
        }
    }

    bak = (VO_S32)(((out_height - 1) * y_scale) >> 10);
    *(scale_tab++) = bak;
    *(scale_tab++) = bak;
    *(scale_tab++) = bak;
}

static void init_scale_bilinear(double x_scale, VO_S32 out_width, VO_S32 *scale_tab, VO_S32 in_width)
{
    VO_S32 i, tmp, par;
    float out_pos;

    for(i = 0; i < out_width; i++)
    {
        out_pos = (float)x_scale * i;
        tmp = (VO_S32)out_pos;
        par = (VO_S32)((out_pos - tmp) * 1024); //1024

        if(tmp >= in_width)
        {
            tmp = in_width - 1;
            par = 512;
        }

        *(scale_tab++) = tmp; // out_pos
        *(scale_tab++) = par; // x
        *(scale_tab++) = ((1 << 10) - par); // y
    }

}

#ifdef COLCONVENABLE
static void init_scale_antialias_level3(double x_scale, VO_S32 out_width, VO_S32 *scale_tab, VO_S32 in_width, VO_S32 width_scale)
{
    VO_S32 i, tmp;
    VO_S32 out_pos, k;
    VO_S32 nPoint = 1;

    if(x_scale >= 8)
        nPoint = (VO_S32)(x_scale / 4);

    for(i = 0; i < out_width; i++)
    {
        out_pos = width_scale * i;

        tmp = out_pos / SCALE_CONST + 1;

        if(tmp > in_width)
            tmp = - 1;
        for(k = 0; k < 15; k++)
        {
            VO_S32 n1 = tmp + k * nPoint;

            if(tmp >= 0 && n1 <= in_width && k * nPoint < (VO_S32)x_scale)
                scale_tab[k] = n1; // out_pos
            else
                scale_tab[k] = -1;
        }
        scale_tab += 15;
    }

}

static void init_scale_antialias_level12(double x_scale, VO_S32 out_width, VO_S32 *scale_tab, VO_S32 in_width, VO_S32 anti_level)
{
    VO_S32 i, tmp, par;
    float out_pos;

    for(i = 0; i < out_width; i++)
    {
        int b, c;
        float a;
        out_pos = x_scale * i;

        tmp = (int)out_pos;
        par = (int)((out_pos - tmp) * 1024); //1024

        if(tmp >= in_width)
        {
            tmp = in_width - 1;
            par = 512;
        }

        *(scale_tab++) = tmp; // out_pos
        *(scale_tab++) = par; // x
        *(scale_tab++) = ((1 << 10) - par); // y

        a = out_pos - anti_alias[anti_level][0];

        b = (int)a;
        if(b < 0)
        {
            *(scale_tab++) = tmp; // out_pos
            *(scale_tab++) = par; // x
            *(scale_tab++) = ((1 << 10) - par); //
        }
        else
        {
            c = (int)((a - b) * 1024); //1024

            if(b >= in_width)
            {
                b = in_width - 1;
                c = 512;
            }

            *(scale_tab++) = b; // out_pos
            *(scale_tab++) = c; // x
            *(scale_tab++) = ((1 << 10) - c); //
        }

        a = out_pos + anti_alias[anti_level][1];
        b = (int)a;
        if(b < 0)
        {
            *(scale_tab++) = tmp; // out_pos
            *(scale_tab++) = par; // x
            *(scale_tab++) = ((1 << 10) - par); //
        }
        else
        {
            c = (int)((a - b) * 1024); //1024

            if(b >= in_width)
            {
                b = in_width - 1;
                c = 512;
            }

            *(scale_tab++) = b; // out_pos
            *(scale_tab++) = c; // x
            *(scale_tab++) = ((1 << 10) - c); //
        }
        a = out_pos + anti_alias[anti_level][0];

        b = (int)a;
        if(b < 0)
        {
            *(scale_tab++) = tmp; // out_pos
            *(scale_tab++) = par; // x
            *(scale_tab++) = ((1 << 10) - par); //
        }
        else
        {
            c = (int)((a - b) * 1024); //1024

            if(b >= in_width)
            {
                b = in_width - 1;
                c = 512;
            }

            *(scale_tab++) = b; // out_pos
            *(scale_tab++) = c; // x
            *(scale_tab++) = ((1 << 10) - c); //
        }

        a = out_pos - anti_alias[anti_level][1];

        b = (int)a;
        if(b < 0)
        {
            *(scale_tab++) = tmp; // out_pos
            *(scale_tab++) = par; // x
            *(scale_tab++) = ((1 << 10) - par); //
        }
        else
        {
            c = (int)((a - b) * 1024); //1024

            if(b >= in_width)
            {
                b = in_width - 1;
                c = 512;
            }

            *(scale_tab++) = b; // out_pos
            *(scale_tab++) = c; // x
            *(scale_tab++) = ((1 << 10) - c); //
        }

    }

}
#endif //COLCONVENABLE

VO_U32 ccGetMemSize(struct CC_HND *cc_hnd, ClrConvData *conv_data)
{
    VO_U32 mem_size = 0;
    VO_S32 out_width = conv_data->nOutWidth;
    VO_S32 out_height = conv_data->nOutHeight;
    VO_S32 width_scale, height_scale, i;
    VO_U32 isPlanar = 0;

    VO_S32 doconversion = !(conv_data->nInType == conv_data->nOutType);

    double XScale = (double)conv_data->nInWidth / (double)conv_data->nOutWidth;
    double YScale = (double)conv_data->nInHeight / (double)conv_data->nOutHeight;

    if ((VO_COLOR_YUV_PLANAR420 == conv_data->nInType) && (VO_COLOR_YUV_420_PACK == conv_data->nOutType || VO_COLOR_YUV_420_PACK_2 == conv_data->nOutType))
    {
        doconversion = 0;
    }

    if(((VO_U32)(conv_data->nIsResize) > 1) || ((VO_U32)(conv_data->nRotationType) > 7) ||
            ((VO_U32)(conv_data->nAntiAliasLevel) > 3))
        return 0;


    /* yuv nv12 convert to yuv 420 planar first*/
    if((conv_data->nInType == VO_COLOR_YUV_420_PACK) || (conv_data->nInType == VO_COLOR_YUV_420_PACK_2))
    {
        mem_size += ((conv_data->nInHeight * conv_data->nInStride / 4) + SAFETY + CACHE_LINE);
        mem_size += ((conv_data->nInHeight * conv_data->nInStride / 4) + SAFETY + CACHE_LINE);
    }
#ifdef COLCONVENABLE
    else if((conv_data->nOutType == VO_COLOR_YUV_420_PACK) || (conv_data->nOutType == VO_COLOR_YUV_420_PACK_2))
    {
        if ((ROTATION_90L == conv_data->nRotationType) || (ROTATION_90R == conv_data->nRotationType))
        {
            mem_size += ((conv_data->nOutWidth * conv_data->nOutStride / 4) + SAFETY + CACHE_LINE);
            mem_size += ((conv_data->nOutWidth * conv_data->nOutStride / 4) + SAFETY + CACHE_LINE);
        }
        else
        {
            mem_size += ((conv_data->nOutHeight * conv_data->nOutStride / 4) + SAFETY + CACHE_LINE);
            mem_size += ((conv_data->nOutHeight * conv_data->nOutStride / 4) + SAFETY + CACHE_LINE);
        }
    }
#endif // COLCONVENABLE

#ifdef VOARMV7
    if((!conv_data->nIsResize) && (ROTATION_DISABLE != conv_data->nRotationType) &&
            (TP_IS_PLANAR_420_422(conv_data->nInType) || (conv_data->nInType == VO_COLOR_YUV_420_PACK || conv_data->nInType == VO_COLOR_YUV_420_PACK_2)))
    {
        mem_size += ((8 * 8 * 3 + 8) * sizeof(VO_U8) + CACHE_LINE);
    }
#endif

#ifdef VOX86
    if((!conv_data->nIsResize) && (ROTATION_DISABLE != conv_data->nRotationType) &&
            (TP_IS_PLANAR_420_422(conv_data->nInType) || (conv_data->nInType == VO_COLOR_YUV_420_PACK || conv_data->nInType == VO_COLOR_YUV_420_PACK_2)))
    {
        mem_size += ((16 * 16 * 3 + 16) * sizeof(VO_U8) + CACHE_LINE) * cc_hnd->ClrConvEnvP->thdNum;
    }
#endif


    if(conv_data->nIsResize)
    {
        //VO_S32 *x_resize_tab , *y_resize_tab;

        width_scale  = (VO_S32)(XScale * SCALE_CONST);
        height_scale = (VO_S32)(YScale * SCALE_CONST);

        /*Do resize conversion and rotation need temp buffer*/
        if((conv_data->nIsResize) && (conv_data->nRotationType != ROTATION_DISABLE) && doconversion && \
                (!TP_IS_PLANAR_420_422(conv_data->nInType)) && (VO_COLOR_RGB888_PLANAR != conv_data->nInType) && \
                (VO_COLOR_RGB888_PACKED != conv_data->nInType) && (VO_COLOR_YUV_420_PACK != conv_data->nInType) && \
                (VO_COLOR_YUV_420_PACK_2 != conv_data->nInType))
        {
            cc_hnd->conv_internal = *conv_data;

            mem_size += (conv_data->nInHeight * conv_data->nInWidth + SAFETY + CACHE_LINE);
            mem_size += (conv_data->nInHeight * conv_data->nInWidth / 4 + SAFETY + CACHE_LINE);
            mem_size += (conv_data->nInHeight * conv_data->nInWidth / 4 + SAFETY + CACHE_LINE);

            width_scale  = (VO_S32)(YScale * SCALE_CONST);
            height_scale = (VO_S32)(XScale * SCALE_CONST);
        }

        isPlanar = TP_IS_PLANAR(conv_data->nInType);

        if((isPlanar  && (!conv_data->nAntiAliasLevel) && doconversion && (VO_COLOR_RGB888_PACKED != conv_data->nOutType)) ||
                ((VO_COLOR_RGB565_PACKED == conv_data->nInType) && !doconversion) ||
                ((VO_COLOR_RGB565_PACKED == conv_data->nOutType) && (VO_COLOR_RGB888_PACKED == conv_data->nInType)) ||
                ((VO_COLOR_YUV_420_PACK == conv_data->nInType) && (conv_data->nInType != conv_data->nOutType) && (VO_COLOR_RGB888_PACKED != conv_data->nOutType)) ||
                ((VO_COLOR_YUV_420_PACK_2 == conv_data->nInType) && (conv_data->nInType != conv_data->nOutType) && (VO_COLOR_RGB888_PACKED != conv_data->nOutType)) ||
                (VO_COLOR_RGB32_PACKED == conv_data->nOutType) || (VO_COLOR_ARGB32_PACKED == conv_data->nOutType))
        {
            const VO_S32 in_width  = conv_data->nInWidth;
            const VO_S32 in_height = conv_data->nInHeight;

#if defined(VOARMV7)
            VO_S32 out_width_tmp = 0, out_height_tmp = 0;

            if((VO_COLOR_YUV_PLANAR420 == conv_data->nInType) || (VO_COLOR_YVU_PLANAR420 == conv_data->nInType) ||
                    (VO_COLOR_YUV_PLANAR422_12 == conv_data->nInType) || (VO_COLOR_YVU_PLANAR422_12 == conv_data->nInType) ||
                    ((VO_COLOR_RGB888_PLANAR == conv_data->nInType) && (ROTATION_DISABLE == conv_data->nRotationType)) ||
                    (VO_COLOR_YUV_420_PACK == conv_data->nInType) || (VO_COLOR_YUV_420_PACK_2 == conv_data->nInType))
            {

                if(VO_COLOR_RGB888_PLANAR != conv_data->nInType)
                {
                    mem_size += ((8 * 8 * 3 + 8) * sizeof(VO_U8) + CACHE_LINE);
                }

                out_width_tmp = (out_width + 7) & 0xfffffff8;
                out_height_tmp =  (out_height + 7) & 0xfffffff8;

                mem_size += (((out_width_tmp / 8) + out_width_tmp * 3 + 2) * sizeof(VO_S32) + CACHE_LINE);
                mem_size += (((out_height_tmp / 8) + out_height_tmp * 3 + 2) * sizeof(VO_S32) + CACHE_LINE);
            }
            else
            {
                mem_size += (((in_width / 16) + out_width * 3 + 2) * sizeof(VO_S32) + CACHE_LINE);
                mem_size += (((in_height / 16) + out_height * 3 + 2) * sizeof(VO_S32) + CACHE_LINE);
            }
#elif defined(VOX86)
            VO_S32 out_width_tmp = 0, out_height_tmp = 0;

            if((VO_COLOR_YUV_PLANAR420 == conv_data->nInType) || (VO_COLOR_YVU_PLANAR420 == conv_data->nInType) ||
                    (VO_COLOR_YUV_PLANAR422_12 == conv_data->nInType) || (VO_COLOR_YVU_PLANAR422_12 == conv_data->nInType) ||
                    ((VO_COLOR_RGB888_PLANAR == conv_data->nInType) && (ROTATION_DISABLE == conv_data->nRotationType)) ||
                    (VO_COLOR_YUV_420_PACK == conv_data->nInType) || (VO_COLOR_YUV_420_PACK_2 == conv_data->nInType))
            {

                if(VO_COLOR_RGB888_PLANAR != conv_data->nInType)
                {
                    mem_size += ((16 * 16 * 3 + 16) * sizeof(VO_U8) + CACHE_LINE) * cc_hnd->ClrConvEnvP->thdNum;
                }

                out_width_tmp = (out_width + 15) / 16 * 16;
                out_height_tmp =  (out_height + 15) / 16 * 16;

                mem_size += (((out_width_tmp / 16) + out_width_tmp * 3 + 2) * sizeof(VO_S32) + CACHE_LINE);
                mem_size += (((out_height_tmp / 16) + out_height_tmp * 3 + 2) * sizeof(VO_S32) + CACHE_LINE);
            }
            else
            {
                mem_size += (((in_width / 16) + out_width * 3 + 2) * sizeof(VO_S32) + CACHE_LINE);
                mem_size += (((in_height / 16) + out_height * 3 + 2) * sizeof(VO_S32) + CACHE_LINE);
            }

#else
            mem_size += (((in_width / 16) + out_width * 3 + 2) * sizeof(VO_S32) + CACHE_LINE);
            mem_size += (((in_height / 16) + out_height * 3 + 2) * sizeof(VO_S32) + CACHE_LINE);
#endif
        }
        else
        {
            if(conv_data->nAntiAliasLevel)
            {
                i = 15;
            }
            else
            {
                i = 3;
            }
            mem_size += ((out_width * i + 4) * sizeof(VO_S32) + CACHE_LINE);
            mem_size += ((out_height * i + 4) * sizeof(VO_S32) + CACHE_LINE);
        }
    }

    return mem_size;
}


VOCCRETURNCODE ccIniClrConv(struct CC_HND *cc_hnd, ClrConvData *conv_data)
{
    VO_S32 out_width = conv_data->nOutWidth;
    VO_S32 out_height = conv_data->nOutHeight;
    VO_S32 width_scale, height_scale, i;
    VO_U32 isPlanar = 0;
    VO_U32 mem_size = 0;
    VO_U8  *pMemBuf = NULL;
    VO_U32 rc = VO_ERR_NONE;

    VO_S32 doconversion = !(conv_data->nInType == conv_data->nOutType);

    double XScale = (double)conv_data->nInWidth / (double)conv_data->nOutWidth;
    double YScale = (double)conv_data->nInHeight / (double)conv_data->nOutHeight;

    mem_size = ccGetMemSize(cc_hnd, conv_data);

    if(mem_size)
    {
        cc_hnd->pTotalBuffer = (VO_U8 *)malloc(mem_size);
        if (!cc_hnd->pTotalBuffer)
        {
            goto malloc_failed;
        }
        pMemBuf = cc_hnd->pTotalBuffer;

    }

    if ((VO_COLOR_YUV_PLANAR420 == conv_data->nInType) && (VO_COLOR_YUV_420_PACK == conv_data->nOutType || VO_COLOR_YUV_420_PACK_2 == conv_data->nOutType))
    {
        doconversion = 0;
    }

    cc_hnd->pYUVBuffer = NULL;

    if(((VO_U32)(conv_data->nIsResize) > 1) || ((VO_U32)(conv_data->nRotationType) > 7) ||
            ((VO_U32)(conv_data->nAntiAliasLevel) > 3))
        return VO_ERR_INVALID_ARG;

    if ((!conv_data->nInUVStride) && (TP_IS_PLANAR_420_422(conv_data->nInType)))
    {
        conv_data->nInUVStride = conv_data->nInStride >> 1;
    }
    else if ((!conv_data->nInUVStride) && (VO_COLOR_YUV_PLANAR411 == conv_data->nInType))
    {
        conv_data->nInUVStride = conv_data->nInUVStride >> 2;
    }
    else if ((!conv_data->nInUVStride) && (VO_COLOR_YUV_PLANAR411V == conv_data->nInType || VO_COLOR_YUV_PLANAR444 == conv_data->nInType))
    {
        conv_data->nInUVStride = conv_data->nInUVStride;
    }

    /* yuv nv12 convert to yuv 420 planar first*/
    if((conv_data->nInType == VO_COLOR_YUV_420_PACK) || (conv_data->nInType == VO_COLOR_YUV_420_PACK_2))
    {
        ALIGN_MEM(pMemBuf)
        cc_hnd->yuv_nv12[0] = pMemBuf;
        pMemBuf += ((conv_data->nInHeight * conv_data->nInStride / 4) + SAFETY);

        ALIGN_MEM(pMemBuf)
        cc_hnd->yuv_nv12[1] = pMemBuf;
        pMemBuf += ((conv_data->nInHeight * conv_data->nInStride / 4) + SAFETY);

        if (!conv_data->nInUVStride)
        {
            conv_data->nInUVStride = conv_data->nInStride >> 1;
        }

    }
#ifdef COLCONVENABLE
    else if((conv_data->nOutType == VO_COLOR_YUV_420_PACK) || (conv_data->nOutType == VO_COLOR_YUV_420_PACK_2))
    {

        if ((ROTATION_90L == conv_data->nRotationType) || (ROTATION_90R == conv_data->nRotationType))
        {
            ALIGN_MEM(pMemBuf)
            cc_hnd->yuv_nv12[0] = pMemBuf;
            pMemBuf += ((conv_data->nOutWidth * conv_data->nOutStride / 4) + SAFETY);

            ALIGN_MEM(pMemBuf)
            cc_hnd->yuv_nv12[1] = pMemBuf;
            pMemBuf += ((conv_data->nOutWidth * conv_data->nOutStride / 4) + SAFETY);
        }
        else
        {
            ALIGN_MEM(pMemBuf)
            cc_hnd->yuv_nv12[0] = pMemBuf;
            pMemBuf += ((conv_data->nOutHeight * conv_data->nOutStride / 4) + SAFETY);

            ALIGN_MEM(pMemBuf)
            cc_hnd->yuv_nv12[1] = pMemBuf;
            pMemBuf += ((conv_data->nOutHeight * conv_data->nOutStride / 4) + SAFETY);
        }

        if (!conv_data->nInUVStride)
        {
            conv_data->nInUVStride = conv_data->nInStride >> 1;
        }

    }
#endif // COLCONVENABLE

#ifdef VOARMV7
    if((!conv_data->nIsResize) && (ROTATION_DISABLE != conv_data->nRotationType) &&
            (TP_IS_PLANAR_420_422(conv_data->nInType) || (conv_data->nInType == VO_COLOR_YUV_420_PACK || conv_data->nInType == VO_COLOR_YUV_420_PACK_2)))
    {
        ALIGN_MEM(pMemBuf)
        cc_hnd->pYUVBuffer = pMemBuf;
        pMemBuf += ((8 * 8 * 3 + 8) * sizeof(VO_U8));
    }
#endif

#ifdef VOX86
    if((!conv_data->nIsResize) && (ROTATION_DISABLE != conv_data->nRotationType) &&
            (TP_IS_PLANAR_420_422(conv_data->nInType) || (conv_data->nInType == VO_COLOR_YUV_420_PACK || conv_data->nInType == VO_COLOR_YUV_420_PACK_2)))
    {
        ALIGN_MEM(pMemBuf)
        cc_hnd->pYUVBuffer = pMemBuf;
        pMemBuf += ((16 * 16 * 3 + 16) * sizeof(VO_U8)) * cc_hnd->ClrConvEnvP->thdNum;
	 cc_hnd->ClrConvEnvP->thdBufSize = ((16 * 16 * 3 + 16) * sizeof(VO_U8));	
    }
#endif


    if(conv_data->nIsResize)
    {
        VO_S32 *x_resize_tab , *y_resize_tab;

        width_scale  = (VO_S32)(XScale * SCALE_CONST);
        height_scale = (VO_S32)(YScale * SCALE_CONST);

        /*Do resize conversion and rotation need temp buffer*/
        if((conv_data->nIsResize) && (conv_data->nRotationType != ROTATION_DISABLE) && doconversion && \
                (!TP_IS_PLANAR_420_422(conv_data->nInType)) && (VO_COLOR_RGB888_PLANAR != conv_data->nInType) && \
                (VO_COLOR_RGB888_PACKED != conv_data->nInType) && (VO_COLOR_YUV_420_PACK != conv_data->nInType) && \
                (VO_COLOR_YUV_420_PACK_2 != conv_data->nInType))
        {
            cc_hnd->conv_internal = *conv_data;

            ALIGN_MEM(pMemBuf)
            cc_hnd->scale_buf[0] = pMemBuf;
            pMemBuf += (conv_data->nInHeight * conv_data->nInWidth + SAFETY);

            ALIGN_MEM(pMemBuf)
            cc_hnd->scale_buf[1] = pMemBuf;
            pMemBuf += (conv_data->nInHeight * conv_data->nInWidth / 4 + SAFETY);

            ALIGN_MEM(pMemBuf)
            cc_hnd->scale_buf[2] = pMemBuf;
            pMemBuf += (conv_data->nInHeight * conv_data->nInWidth / 4 + SAFETY);

            /* initialize the temp conversion structure*/
            cc_hnd->conv_internal.nInWidth = conv_data->nInHeight;
            cc_hnd->conv_internal.nInHeight = conv_data->nInWidth;
            cc_hnd->conv_internal.nOutWidth = conv_data->nOutHeight;
            cc_hnd->conv_internal.nOutHeight = conv_data->nOutWidth;
            cc_hnd->conv_internal.nInStride = conv_data->nInHeight;

            out_width = conv_data->nOutHeight;
            out_height = conv_data->nOutWidth;
            width_scale  = (VO_S32)(YScale * SCALE_CONST);
            height_scale = (VO_S32)(XScale * SCALE_CONST);
        }

        isPlanar = TP_IS_PLANAR(conv_data->nInType);

        if((isPlanar  && (!conv_data->nAntiAliasLevel) && doconversion && (VO_COLOR_RGB888_PACKED != conv_data->nOutType)) ||
                ((VO_COLOR_RGB565_PACKED == conv_data->nInType) && !doconversion) ||
                ((VO_COLOR_RGB565_PACKED == conv_data->nOutType) && (VO_COLOR_RGB888_PACKED == conv_data->nInType)) ||
                ((VO_COLOR_YUV_420_PACK == conv_data->nInType) && (conv_data->nInType != conv_data->nOutType) && (VO_COLOR_RGB888_PACKED != conv_data->nOutType)) ||
                ((VO_COLOR_YUV_420_PACK_2 == conv_data->nInType) && (conv_data->nInType != conv_data->nOutType) && (VO_COLOR_RGB888_PACKED != conv_data->nOutType)) ||
                (VO_COLOR_RGB32_PACKED == conv_data->nOutType) || (VO_COLOR_ARGB32_PACKED == conv_data->nOutType))
        {
            const VO_S32 in_width  = conv_data->nInWidth;
            const VO_S32 in_height = conv_data->nInHeight;
#if defined(VOARMV7)
            if((VO_COLOR_YUV_PLANAR420 == conv_data->nInType) || (VO_COLOR_YVU_PLANAR420 == conv_data->nInType) ||
                    (VO_COLOR_YUV_PLANAR422_12 == conv_data->nInType) || (VO_COLOR_YVU_PLANAR422_12 == conv_data->nInType) ||
                    ((VO_COLOR_RGB888_PLANAR == conv_data->nInType) && (ROTATION_DISABLE == conv_data->nRotationType)) ||
                    (VO_COLOR_YUV_420_PACK == conv_data->nInType) || (VO_COLOR_YUV_420_PACK_2 == conv_data->nInType))
            {
                VO_S32 out_width_tmp = 0, out_height_tmp = 0;

                if(VO_COLOR_RGB888_PLANAR != conv_data->nInType)
                {
                    ALIGN_MEM(pMemBuf)
                    cc_hnd->pYUVBuffer = pMemBuf;
                    pMemBuf += ((8 * 8 * 3 + 8) * sizeof(VO_U8));
                }

                out_width_tmp = (out_width + 7) & 0xfffffff8;
                out_height_tmp =  (out_height + 7) & 0xfffffff8;

                ALIGN_MEM(pMemBuf)
                x_resize_tab = (VO_S32 *)pMemBuf;
                pMemBuf += (((out_width_tmp / 8) + out_width_tmp * 3 + 2) * sizeof(VO_S32));

                ALIGN_MEM(pMemBuf)
                y_resize_tab = (VO_S32 *)pMemBuf;
                pMemBuf += (((out_height_tmp / 8) + out_height_tmp * 3 + 2) * sizeof(VO_S32));

                memset(x_resize_tab, 0, ((out_width_tmp / 8) + out_width_tmp * 3 + 2)*sizeof(int));
                memset(y_resize_tab, 0, ((out_height_tmp / 8) + out_height_tmp * 3 + 2)*sizeof(int));

                init_scale_VOARMV7_width2((float)in_width / out_width, out_width_tmp, x_resize_tab, in_width);
                init_scale_VOARMV7_height2((float)in_height / out_height, out_height, y_resize_tab, in_height);
            }
            else
            {
                ALIGN_MEM(pMemBuf)
                x_resize_tab  = (VO_S32 *)pMemBuf;
                pMemBuf += (((in_width / 16) + out_width * 3 + 2) * sizeof(VO_S32));

                ALIGN_MEM(pMemBuf)
                y_resize_tab  = (VO_S32 *)pMemBuf;
                pMemBuf += (((in_height / 16) + out_height * 3 + 2) * sizeof(VO_S32));

                memset(x_resize_tab, 0, ((in_width / 16) + out_width * 3 + 2) * sizeof(VO_S32));
                memset(y_resize_tab, 0, ((in_height / 16) + out_height * 3 + 2) * sizeof(VO_S32));
                init_scale((float)in_width / out_width, out_width, x_resize_tab, in_width);
                init_scale((float)in_height / out_height, out_height, y_resize_tab, in_height);
            }
#elif defined(VOX86)
            if((VO_COLOR_YUV_PLANAR420 == conv_data->nInType) || (VO_COLOR_YVU_PLANAR420 == conv_data->nInType) ||
                    (VO_COLOR_YUV_PLANAR422_12 == conv_data->nInType) || (VO_COLOR_YVU_PLANAR422_12 == conv_data->nInType) ||
                    ((VO_COLOR_RGB888_PLANAR == conv_data->nInType) && (ROTATION_DISABLE == conv_data->nRotationType)) ||
                    (VO_COLOR_YUV_420_PACK == conv_data->nInType) || (VO_COLOR_YUV_420_PACK_2 == conv_data->nInType))
            {
                VO_S32 out_width_tmp = 0, out_height_tmp = 0;

                if(VO_COLOR_RGB888_PLANAR != conv_data->nInType)
                {
                    ALIGN_MEM(pMemBuf)
                    cc_hnd->pYUVBuffer = pMemBuf;
                    pMemBuf += ((16 * 16 * 3 + 16) * sizeof(VO_U8)) * cc_hnd->ClrConvEnvP->thdNum;
		      cc_hnd->ClrConvEnvP->thdBufSize = ((16 * 16 * 3 + 16) * sizeof(VO_U8));					
                }

                out_width_tmp = (out_width + 15) / 16 * 16;
                out_height_tmp =  (out_height + 15) / 16 * 16;

                ALIGN_MEM(pMemBuf)
                x_resize_tab = (VO_S32 *)pMemBuf;
                pMemBuf += (((out_width_tmp / 16) + out_width_tmp * 3 + 2) * sizeof(VO_S32));

                ALIGN_MEM(pMemBuf)
                y_resize_tab = (VO_S32 *)pMemBuf;
                pMemBuf += (((out_height_tmp / 16) + out_height_tmp * 3 + 2) * sizeof(VO_S32));

                memset(x_resize_tab, 0, ((out_width_tmp / 16) + out_width_tmp * 3 + 2)*sizeof(int));
                memset(y_resize_tab, 0, ((out_height_tmp / 16) + out_height_tmp * 3 + 2)*sizeof(int));


                init_scale_VOX86_width2((float)in_width / out_width, out_width_tmp, x_resize_tab, in_width);
                init_scale_VOX86_height2((float)in_height / out_height, out_height, y_resize_tab, in_height);

#ifdef VOX86
                cc_hnd->ClrConvEnvP->x_resize_tab = x_resize_tab; 
                cc_hnd->ClrConvEnvP->y_resize_tab = y_resize_tab; 
		  cc_hnd->ClrConvEnvP->totBlkLine = out_height_tmp  / 16;
		  cc_hnd->ClrConvEnvP->curBlkLine = 0;
		  cc_hnd->ClrConvEnvP->height_remainder = out_height_tmp -  out_height;
#endif				
				
            }
            else
            {
                ALIGN_MEM(pMemBuf)
                x_resize_tab  = (VO_S32 *)pMemBuf;
                pMemBuf += (((in_width / 16) + out_width * 3 + 2) * sizeof(VO_S32));

                ALIGN_MEM(pMemBuf)
                y_resize_tab  = (VO_S32 *)pMemBuf;
                pMemBuf += (((in_height / 16) + out_height * 3 + 2) * sizeof(VO_S32));

                memset(x_resize_tab, 0, ((in_width / 16) + out_width * 3 + 2) * sizeof(VO_S32));
                memset(y_resize_tab, 0, ((in_height / 16) + out_height * 3 + 2) * sizeof(VO_S32));
                init_scale((float)in_width / out_width, out_width, x_resize_tab, in_width);
                init_scale((float)in_height / out_height, out_height, y_resize_tab, in_height);
            }

#else
            ALIGN_MEM(pMemBuf)
            x_resize_tab  = (VO_S32 *)pMemBuf;
            pMemBuf += (((in_width / 16) + out_width * 3 + 2) * sizeof(VO_S32));

            ALIGN_MEM(pMemBuf)
            y_resize_tab  = (VO_S32 *)pMemBuf;
            pMemBuf += (((in_height / 16) + out_height * 3 + 2) * sizeof(VO_S32));

            memset(x_resize_tab, 0, ((in_width / 16) + out_width * 3 + 2) * sizeof(VO_S32));
            memset(y_resize_tab, 0, ((in_height / 16) + out_height * 3 + 2) * sizeof(VO_S32));

            init_scale((float)in_width / out_width, out_width, x_resize_tab, in_width);
            init_scale((float)in_height / out_height, out_height, y_resize_tab, in_height);

#endif
        }
        else
        {
            //TBD
            if(conv_data->nAntiAliasLevel)
            {
                i = 15;
            }
            else
            {
                i = 3;
            }

            ALIGN_MEM(pMemBuf)
            x_resize_tab = (VO_S32 *)pMemBuf;
            pMemBuf += ((out_width * i + 4) * sizeof(VO_S32));
            memset(x_resize_tab, 0, (out_width * i + 4) * sizeof(VO_S32));

            ALIGN_MEM(pMemBuf)
            y_resize_tab = (VO_S32 *)pMemBuf;
            pMemBuf += (out_height * i + 4) * sizeof(VO_S32);
            memset(y_resize_tab, 0, (out_height * i + 4) * sizeof(VO_S32));

            if((doconversion && !isPlanar) || (TP_IS_PLANAR_420(conv_data->nInType) && !doconversion && ROTATION_DISABLE != conv_data->nRotationType) ||
                    (TP_IS_PLANAR_420_422(conv_data->nInType) && (VO_COLOR_RGB888_PACKED == conv_data->nOutType)) ||
                    ((VO_COLOR_YUYV422_PACKED == conv_data->nInType || conv_data->nInType == VO_COLOR_YVYU422_PACKED || conv_data->nInType == VO_COLOR_UYVY422_PACKED ||
                      conv_data->nInType == VO_COLOR_VYUY422_PACKED || conv_data->nInType == VO_COLOR_YUYV422_PACKED_2 || conv_data->nInType == VO_COLOR_YVYU422_PACKED_2 ||
                      conv_data->nInType == VO_COLOR_UYVY422_PACKED_2 || conv_data->nInType == VO_COLOR_VYUY422_PACKED_2 ) && (ROTATION_DISABLE != conv_data->nRotationType)))
            {
                if(conv_data->nInType == VO_COLOR_YUYV422_PACKED || conv_data->nInType == VO_COLOR_YVYU422_PACKED ||
                        conv_data->nInType == VO_COLOR_UYVY422_PACKED || conv_data->nInType == VO_COLOR_VYUY422_PACKED ||
                        conv_data->nInType == VO_COLOR_YUYV422_PACKED_2 || conv_data->nInType == VO_COLOR_YVYU422_PACKED_2 ||
                        conv_data->nInType == VO_COLOR_UYVY422_PACKED_2 || conv_data->nInType == VO_COLOR_VYUY422_PACKED_2 ||
                        (TP_IS_PLANAR_420(conv_data->nInType) && (VO_COLOR_RGB888_PACKED != conv_data->nOutType)))
                {

                    init_scale_nearest(width_scale, out_width, x_resize_tab);
                    init_scale_nearest(height_scale, out_height, y_resize_tab);

                }
                else
                {

                    init_scale_nearest_new(width_scale, out_width, x_resize_tab);
                    init_scale_bilinear_new(height_scale, out_height, y_resize_tab);
                }
            }
            else
            {
                //bilinear
                if(conv_data->nInType == VO_COLOR_YUYV422_PACKED || conv_data->nInType == VO_COLOR_YVYU422_PACKED ||
                        conv_data->nInType == VO_COLOR_UYVY422_PACKED || conv_data->nInType == VO_COLOR_VYUY422_PACKED ||
                        conv_data->nInType == VO_COLOR_YUYV422_PACKED_2 || conv_data->nInType == VO_COLOR_YVYU422_PACKED_2 ||
                        conv_data->nInType == VO_COLOR_UYVY422_PACKED_2 || conv_data->nInType == VO_COLOR_VYUY422_PACKED_2  ||
                        conv_data->nInType == VO_COLOR_YUV_PLANAR420 || conv_data->nInType == VO_COLOR_RGB565_PACKED || isPlanar)
                {

                    if(!conv_data->nAntiAliasLevel)
                    {
                        init_scale_bilinear(XScale, out_width, x_resize_tab, conv_data->nInWidth - 1);
                        init_scale_bilinear(YScale, out_height, y_resize_tab, conv_data->nInHeight - 1);
                    }
#ifdef COLCONVENABLE
                    else if (3 == conv_data->nAntiAliasLevel)
                    {
                        init_scale_antialias_level3(XScale, out_width, x_resize_tab, conv_data->nInWidth - 1, width_scale);
                        init_scale_antialias_level3(YScale, out_height, y_resize_tab, conv_data->nInHeight - 1, height_scale);
                    }
                    else
                    {
                        init_scale_antialias_level12(XScale, out_width, x_resize_tab, conv_data->nInWidth - 1, conv_data->nAntiAliasLevel - 1);
                        init_scale_antialias_level12(YScale, out_height, y_resize_tab, conv_data->nInHeight - 1, conv_data->nAntiAliasLevel - 1);
                    }
#endif //COLCONVENABLE
                }
                else
                {
                    init_scale_nearest(width_scale, out_width, x_resize_tab);
                    init_scale_nearest(height_scale, out_height, y_resize_tab);
                }

            }
        }

        cc_hnd->x_resize_tab = x_resize_tab;
        cc_hnd->y_resize_tab = y_resize_tab;
    }

#ifdef VOX86
	cc_hnd->ClrConvEnvP->totBlkLine = (out_height + 15) / 16;
	cc_hnd->ClrConvEnvP->curBlkLine = 0;
	cc_hnd->ClrConvEnvP->height_remainder = (out_height + 15) / 16 * 16 - out_height;
#endif					

    rc = ccGetFunPtr(cc_hnd, conv_data);

    return rc;

malloc_failed:
    if(cc_hnd)
    {
        free(cc_hnd);
        cc_hnd = NULL;
    }
    return VO_ERR_OUTOF_MEMORY;
}

VOCCRETURNCODE ccEndClrConv(struct CC_HND *cc_hnd)
{
    if(cc_hnd->pTotalBuffer)
    {
        free(cc_hnd->pTotalBuffer);
        cc_hnd->pTotalBuffer = NULL;
    }

    if(cc_hnd)
    {
        free(cc_hnd);
        cc_hnd = NULL;
    }

    return VO_ERR_NONE;
}

#ifdef VOX86
void cpuID(unsigned i, unsigned regs[4])
{
#ifdef _WIN32
  __cpuid((int *)regs, (int)i);

#else
  asm volatile
    ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
     : "a" (i), "c" (0));
  // ECX is set to zero for CPUID function 4
#endif
}

VO_S32 GetCpuCores()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return si.dwNumberOfProcessors;
}
#endif

VO_U32 VO_API voCCInit(VO_HANDLE *hCodec, VO_CODEC_INIT_USERDATA *pUserData)
{
    struct CC_HND *cc_hnd;//huwei 20100601 fix a bug
#ifdef VOX86
    VO_S32 i;
#endif    

    if(!((*hCodec) = (struct CC_HND *) malloc(sizeof(struct CC_HND))))
        return VO_ERR_OUTOF_MEMORY;

#if DEBUG_DLL
      {
		  FILE *f;
		  VO_S8 buff[128];
		  f = fopen("D:/cc.log", "a+");
		  sprintf(buff, "voCCInit aaaaaaaaaaaa \n");
		  fputs(buff, f);
		  fclose(f);
       }
#endif	
	

    memset((*hCodec), 0, sizeof(struct CC_HND));

    cc_hnd = (struct CC_HND *)(*hCodec);
    cc_hnd->yuv2rgbmatrix = param_tab[0];

#ifdef VOX86
    if(!(cc_hnd->ClrConvEnvP = malloc(sizeof(ClrConvEnv))))
        return VO_ERR_OUTOF_MEMORY;	
   memset(cc_hnd->ClrConvEnvP, 0, sizeof(ClrConvEnv));

   cc_hnd->ClrConvEnvP->cc_hnd = cc_hnd;
   cc_hnd->ClrConvEnvP->bExit = 0;
   cc_hnd->ClrConvEnvP->conv_data = &cc_hnd->conv_internal;
   cc_hnd->ClrConvEnvP->thdNum = GetCpuCores();
   pthread_mutex_init(&cc_hnd->ClrConvEnvP->clrconv_mutex, 0);
   pthread_cond_init(&cc_hnd->ClrConvEnvP->clrconv_cond, 0);
   pthread_cond_init(&cc_hnd->ClrConvEnvP->cc_end_cond, 0);   
//   vo_sem_init("cc_end_cond", &cc_hnd->ClrConvEnvP->cc_end_cond, 0, 0);

   for(i = 1; i < cc_hnd->ClrConvEnvP->thdNum; i++)
   {
        cc_hnd->ClrConvEnvP->thdIdx = i;
	 cc_hnd->ClrConvEnvP->bRunning = 0;	
       pthread_create((pthread_t *)&cc_hnd->ClrConvEnvP->thread_id[i], 0, ClrConvProc, cc_hnd->ClrConvEnvP);
	 while (!(cc_hnd->ClrConvEnvP->bRunning));
//	 Sleep(1);
//	sem_wait(&cc_hnd->ClrConvEnvP->cc_end_cond);
   }
#if DEBUG_DLL
      {
		  FILE *f;
		  VO_S8 buff[128];
		  f = fopen("D:/cc.log", "a+");
		  sprintf(buff, "voCCInit bbbbbb \n");
		  fputs(buff, f);
		  fclose(f);
       }
#endif	

#endif

    return VO_ERR_NONE;
}


/**
 * Convert or scale or crop one frame data.
 * \param hCodec [in] CC Encoder instance handle, returned by voCCInit().
 * \param pInData [in] Input buffer pointer.
 * \param pOutData [in] Output buffer pointer.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_STATUS The Encoder is not ready to encode. Usually this indicates more parameters needed. See voCCSetParameter().
 */
VO_U32 VO_API voCCProcess(VO_HANDLE hCodec, VO_PTR pData)
{
    VOCCRETURNCODE	nCcRC;
    ClrConvData *conv_data = (ClrConvData *)pData;
    VO_S32  doconversion = 1;
    VO_U8 *dst = conv_data->pOutBuf[0];

    if(((VO_U32)dst) & 3)
    {
        return VO_ERR_INVALID_ARG;
    }

    if ((VO_COLOR_YUV_PLANAR420 == conv_data->nInType) && (VO_COLOR_YUV_420_PACK == conv_data->nOutType  || VO_COLOR_YUV_420_PACK_2 == conv_data->nOutType))
    {
        doconversion = 0;
    }

    if((conv_data->nInType == conv_data->nOutType) || !doconversion)
    {
        nCcRC = ccResizeCropFrame((struct CC_HND *)hCodec, conv_data);
    }
    else
    {
        nCcRC = ccConvAFrame((struct CC_HND *)hCodec, conv_data);
    }

    return nCcRC;
}

/**
 * Close the CC instance, release any resource it used.
 * \param hCodec [in] CC Encoder instance handle, returned by voCCInit().
 * \return The function should always return VORC_OK.
 */
VO_U32 VO_API voCCUninit(VO_HANDLE hCodec)
{
#ifdef VOX86
    VO_S32 i;
    ClrConvEnv *ClrConvEnvP = ((struct CC_HND *)hCodec)->ClrConvEnvP;
#if DEBUG_DLL
      {
		  FILE *f;
		  VO_S8 buff[128];
		  f = fopen("D:/cc.log", "a+");
		  sprintf(buff, "voCCUninit aaaaaaaaaaaa \n");
		  fputs(buff, f);
		  fclose(f);
       }
#endif	


    pthread_mutex_lock(&ClrConvEnvP->clrconv_mutex);
    ClrConvEnvP->bExit = 1;
    ClrConvEnvP->completed = 0;	
    pthread_cond_broadcast(&ClrConvEnvP->clrconv_cond);
    while (ClrConvEnvP->completed != (ClrConvEnvP->thdNum - 1))
    {
        pthread_cond_wait(&ClrConvEnvP->cc_end_cond, &ClrConvEnvP->clrconv_mutex);
	
    }

	for(i = 1; i < ClrConvEnvP->thdNum; i++)
		pthread_join((pthread_t)ClrConvEnvP->thread_id[i],NULL);
    pthread_mutex_unlock(&ClrConvEnvP->clrconv_mutex);		
    pthread_mutex_destroy(&ClrConvEnvP->clrconv_mutex);
    pthread_cond_destroy(&ClrConvEnvP->clrconv_cond);
    pthread_cond_destroy(&ClrConvEnvP->cc_end_cond);	
//    vo_sem_destroy("cc_end_cond", &ClrConvEnvP->cc_end_cond);
    free(ClrConvEnvP);

#if DEBUG_DLL
      {
		  FILE *f;
		  VO_S8 buff[128];
		  f = fopen("D:/cc.log", "a+");
		  sprintf(buff, "voCCUninit bbbbbbb \n");
		  fputs(buff, f);
		  fclose(f);
       }
#endif	
	
#endif
    if(ccEndClrConv((struct CC_HND *)hCodec))
    {

        return VO_ERR_CC_UNINI;
    }

    return VO_ERR_NONE;
}

/**
 * Set parameter of the CC instance.
 * \param hCodec [in] CC instance handle, returned by voCCInit().
 * \param nID [in] Parameter ID, Supports: VOCCPARAMETERID
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
VO_U32 VO_API voCCSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR lValue)
{
    struct CC_HND *cc_hnd = (struct CC_HND *)hCodec;
    VOCCRETURNCODE rt = VO_ERR_NONE;
    VO_S32 i;

    switch(nID)
    {
    case VO_PID_CC_INIINSTANCE:
        rt = ccIniClrConv(cc_hnd, (ClrConvData *)lValue);
        break;
    case VO_PID_CC_CONVPARM:
    {
        VO_S32 matrix_index = (VO_S32)lValue;

        //TBD
        cc_hnd->yuv2rgbmatrix = param_tab[matrix_index];
    }
    break;
    case VO_PID_CC_CONERTMATRIX:

        for(i = 0; i < 4; i++)
        {
            param_tab[2][i] = (VO_S32)(*((double *)lValue + i) * 1024 * 2);
        }

        //TBD
        cc_hnd->yuv2rgbmatrix = param_tab[2];
        break;
    default :
        return VO_ERR_WRONG_PARAM_ID;
    }

    return rt;
}

VO_S32 VO_API voGetClrConvAPI (VO_CLRCONV_DECAPI *pDecHandle, VO_U32 uFlag)
{
    VO_CLRCONV_DECAPI *pClrConv = pDecHandle;

    if(!pClrConv)
        return VO_ERR_INVALID_ARG;

    pClrConv->CCInit     = voCCInit;
    pClrConv->CCUninit   = voCCUninit;
    pClrConv->CCSetParam = voCCSetParameter;
    pClrConv->CCGetParam = NULL;
    pClrConv->CCProcess  = voCCProcess;

    return VO_ERR_NONE;

}

#ifndef COLCONVENABLE

static VOCCRETURNCODE voCCRRRUninit(struct CC_HND *cc_hnd)
{
    if(cc_hnd->pTotalBuffer)
    {
        free(cc_hnd->pTotalBuffer);
        cc_hnd->pTotalBuffer = NULL;
    }

    return VO_ERR_NONE;
}

#if defined(VOSYMBIAN)
extern const unsigned char	ccClip255Base[1024];
extern const unsigned char	ccClip63Base[256];
extern const unsigned char	ccClip31Base[128];
extern unsigned char	*ccClip255;
extern unsigned char	*ccClip63;
extern unsigned char	*ccClip31;
#endif

VO_U32 VO_API voCCRRInit(VO_HANDLE *phCCRR, VO_PTR hView, VO_MEM_OPERATOR *pMemOP, VO_U32 nFlag)
{
    VO_U32 return_code = VO_ERR_NONE;

#ifdef CC_LOGINFO
    VOLOGI("voCCRRInit In ");
#endif

#if defined(VOSYMBIAN)
    ccClip255 = (unsigned char *)( &(ccClip255Base[384]));
    ccClip63 = (unsigned char *)( &(ccClip63Base[96]));
    ccClip31 = (unsigned char *) & (ccClip31Base[48]);
#endif

    return_code = voCCInit(phCCRR, NULL);

#ifdef CC_LOGINFO
    VOLOGI("voCCRRInit Out");
#endif

    return return_code;

}

VO_U32 VO_API voCCRRGetName (VO_HANDLE hCCRR, signed char *pName, int nLen)
{
#ifdef _WIN32
    strcpy_s((char *)pName, 12 , "YYCCRR");
#else  //_WIN32
    strcpy((char *)pName,  "YYCCRR");
#endif //_WIN32
    return VO_ERR_NONE;
}

VO_U32 VO_API voCCRRGetInputType(VO_HANDLE hCCRR, VO_IV_COLORTYPE *pType, VO_U32 nIndex)
{
    if (nIndex == 0)
        *pType = VO_COLOR_RGB565_PACKED;
    else if (nIndex == 1)
        *pType = VO_COLOR_YUV_PLANAR420;
    else if (nIndex == 2)
        *pType = VO_COLOR_RGB888_PACKED;
    else
        return VO_ERR_NOT_IMPLEMENT;

    return VO_ERR_NONE;
}

VO_U32 VO_API voCCRRGetRotateType (VO_HANDLE hCCRR, int *pRotateType)
{
    *pRotateType = 	VO_RT_DISABLE |	VO_RT_90L | VO_RT_90R | VO_RT_180;
    return VO_ERR_NONE;
}


VO_U32 VO_API voCCRRGetProperty (VO_HANDLE hCCRR, VO_CCRRR_PROPERTY *pProperty)
{
    pProperty->nRender = 0;
    pProperty->nOverlay = 0;
    pProperty->nKeyColor = 0;
    pProperty->nRotate = VO_RT_DISABLE | VO_RT_90L | VO_RT_180;
    pProperty->nOutBuffer = 0;
    pProperty->nFlag = 0;

#ifdef _WIN32
    strcpy_s((char *)pProperty->szName, 12 , "YYCCRR");
#else  //_WIN32
    strcpy(pProperty->szName,  "YYCCRR");
#endif //_WIN32
    return VO_ERR_NONE;
}

VO_U32 VO_API voCCRRSetVideoType(VO_HANDLE hCCRR, VO_IV_COLORTYPE lInType, VO_IV_COLORTYPE lOutType)
{
    VOCCRETURNCODE rt = VO_ERR_NONE;
    struct CC_HND *phCR = (struct CC_HND *) hCCRR;

#ifdef CC_LOGINFO
    VOLOGI("voCCRRSetVideoType In");
#endif

    if (phCR == NULL)
        return VO_ERR_INVALID_ARG;

    if(lInType == VO_COLOR_YUV_PLANAR420)
    {
        phCR->conv_internal.nInType = VO_COLOR_YUV_PLANAR420;
    }
    else if(lInType == VO_COLOR_RGB565_PACKED)
    {
        phCR->conv_internal.nInType = VO_COLOR_RGB565_PACKED;
    }
    else if(lInType == VO_COLOR_RGB888_PACKED)
    {
        phCR->conv_internal.nInType = VO_COLOR_RGB888_PACKED;
    }
    else if(lInType == VO_COLOR_YUV_420_PACK)
    {
        phCR->conv_internal.nInType = VO_COLOR_YUV_420_PACK;
    }
    else if(lInType == VO_COLOR_YUV_420_PACK_2)
    {
        phCR->conv_internal.nInType = VO_COLOR_YUV_420_PACK_2;
    }
    else
    {
        return VO_ERR_NOT_IMPLEMENT;
    }
    if(lOutType == VO_COLOR_YUV_PLANAR420)
    {
        phCR->conv_internal.nOutType = VO_COLOR_YUV_PLANAR420;
    }
    else if(lOutType == VO_COLOR_RGB565_PACKED)
    {
        phCR->conv_internal.nOutType = VO_COLOR_RGB565_PACKED;
    }
    else if(lOutType == VO_COLOR_RGB888_PACKED)
    {
        phCR->conv_internal.nOutType = VO_COLOR_RGB888_PACKED;
    }
    else if (lOutType == VO_COLOR_RGB32_PACKED)
    {
        phCR->conv_internal.nOutType = VO_COLOR_RGB32_PACKED;
    }
    else if(lOutType == VO_COLOR_ARGB32_PACKED)
    {
        phCR->conv_internal.nOutType = VO_COLOR_ARGB32_PACKED;
    }
    else if(lOutType == VO_COLOR_YUYV422_PACKED)
    {
        phCR->conv_internal.nOutType = VO_COLOR_YUYV422_PACKED;
    }
    else if(lOutType == VO_COLOR_UYVY422_PACKED)
    {
        phCR->conv_internal.nOutType = VO_COLOR_UYVY422_PACKED;
    }
    else
    {
        return VO_ERR_NOT_IMPLEMENT;
    }
    phCR->ini_flg |= (1 << 5);
    if(phCR->ini_flg == 0x3f)
    {
        ClrConvData conv_data;

        if(phCR->ccrrr_init_flage)
        {
            voCCRRRUninit(phCR);
            phCR->ccrrr_init_flage = 0;
        }

        conv_data.nAntiAliasLevel = NO_ANTIALIASING;
        conv_data.nRotationType   = phCR->conv_internal.nRotationType;
        conv_data.mb_skip         = NULL;

        conv_data.nInType       = phCR->conv_internal.nInType;
        conv_data.nInWidth      = phCR->conv_internal.nInWidth;
        conv_data.nInHeight     = phCR->conv_internal.nInHeight;
        conv_data.nInStride     = phCR->conv_internal.nInWidth;
        conv_data.nInUVStride   = 0;
        conv_data.nOutType      = phCR->conv_internal.nOutType;
        conv_data.nOutWidth     = phCR->conv_internal.nOutWidth;
        conv_data.nOutHeight    = phCR->conv_internal.nOutHeight;
        conv_data.nOutStride    = phCR->conv_internal.nOutStride;
        conv_data.nOutUVStride  = 0;

        if ((conv_data.nInWidth != conv_data.nOutWidth) || (conv_data.nInHeight != conv_data.nOutHeight))
        {
            phCR->conv_internal.nIsResize = 1;
            conv_data.nIsResize           = 1;
        }
        else
        {
            phCR->conv_internal.nIsResize = 0;
            conv_data.nIsResize           = 0;
        }

        rt = ccIniClrConv(phCR, &conv_data);

        phCR->ccrrr_init_flage = 1;
    }

#ifdef CC_LOGINFO
    VOLOGI("lInType = %d, lOutType = %d ", lInType, lOutType);
    VOLOGI("voCCRRSetVideoType Out ");
#endif

    return rt;
}

VO_U32 VO_API voCCRRSetVideoSize(VO_HANDLE hCCRR, VO_U32 *nInWidth, VO_U32 *nInHeight, VO_U32 *nOutWidth, VO_U32 *nOutHeight, VO_IV_RTTYPE nRotateType)
{
    struct CC_HND *phCR = (struct CC_HND *) hCCRR;
    VOCCRETURNCODE rt = VO_ERR_NONE;

    if (phCR == NULL)
        return VO_ERR_INVALID_ARG;

#ifdef CC_LOGINFO
    VOLOGI("voCCRRSetVideoSize In");
    VOLOGI("nInWidth = %d, nInHeight = %d, nOutWidth = %d, nOutHeight = %d, nRotateType = %d", *nInWidth, *nInHeight, *nOutWidth, *nOutHeight, nRotateType);
#endif

    phCR->conv_internal.nInWidth = *nInWidth;
    phCR->conv_internal.nInHeight = *nInHeight;
    phCR->ini_flg |= 1;
    phCR->ini_flg |= (1 << 1);

    phCR->conv_internal.nOutWidth = *nOutWidth ;
    phCR->conv_internal.nOutHeight = *nOutHeight;
    phCR->ini_flg |= (1 << 2);
    phCR->ini_flg |= (1 << 3);

    switch (nRotateType)
    {
    case VO_RT_DISABLE:
        phCR->conv_internal.nRotationType = ROTATION_DISABLE;  /*!< No rotation */
        break;
    case VO_RT_90L:
        phCR->conv_internal.nRotationType = ROTATION_90L;  /*!< Left rotation 90 */
        break;
    case VO_RT_90R:
        phCR->conv_internal.nRotationType = ROTATION_90R;  /*!< Right rotation 90 */
        break;
    case VO_RT_180:
        phCR->conv_internal.nRotationType = ROTATION_180;  /*!< Right rotation 90 */
        break;
    default:
        return VO_ERR_NOT_IMPLEMENT;
        break;
    }
    phCR->ini_flg |= (1 << 4);

    if(phCR->ini_flg == 0x3f)
    {
        ClrConvData conv_data;

        if(phCR->ccrrr_init_flage)
        {
            voCCRRRUninit(phCR);
            phCR->ccrrr_init_flage = 0;
        }

        conv_data.nAntiAliasLevel = NO_ANTIALIASING;
        conv_data.nRotationType   = phCR->conv_internal.nRotationType;
        conv_data.mb_skip         = NULL;

        conv_data.nInType       = phCR->conv_internal.nInType;
        conv_data.nInWidth      = phCR->conv_internal.nInWidth;
        conv_data.nInHeight     = phCR->conv_internal.nInHeight;
        conv_data.nInStride     = phCR->conv_internal.nInWidth;
        conv_data.nInUVStride   = 0;
        conv_data.nOutType      = phCR->conv_internal.nOutType;
        conv_data.nOutWidth     = phCR->conv_internal.nOutWidth;
        conv_data.nOutHeight    = phCR->conv_internal.nOutHeight;
        conv_data.nOutStride    = phCR->conv_internal.nOutStride;
        conv_data.nOutUVStride  = 0;

        if ((conv_data.nInWidth != conv_data.nOutWidth) || (conv_data.nInHeight != conv_data.nOutHeight))
        {
            phCR->conv_internal.nIsResize = 1;
            conv_data.nIsResize           = 1;
        }
        else
        {
            phCR->conv_internal.nIsResize = 0;
            conv_data.nIsResize           = 0;
        }

        if((phCR->conv_internal.nInType = VO_COLOR_YUV_PLANAR420) &&
                ((phCR->conv_internal.nOutType == VO_COLOR_YUYV422_PACKED) || (phCR->conv_internal.nOutType == VO_COLOR_UYVY422_PACKED)))
        {
            if((phCR->conv_internal.nRotationType != ROTATION_DISABLE) || (phCR->conv_internal.nIsResize == 1))
            {
                return VO_ERR_NOT_IMPLEMENT;
            }
        }

        rt = ccIniClrConv(phCR, &conv_data);

        phCR->ccrrr_init_flage = 1;
    }

#ifdef CC_LOGINFO
    VOLOGI("voCCRRSetVideoSize Out");
#endif

    return rt;

}

VO_U32 VO_API voCCRRSetWindow(VO_HANDLE hCCRR, void *hWnd)
{
    return VO_ERR_FAILED;
}

VO_U32 VO_API voCCRRGetOutputType(VO_HANDLE hCCRR, VO_IV_COLORTYPE *pType, VO_U32 nIndex)
{
    if (nIndex == 0)
        *pType = VO_COLOR_RGB565_PACKED;
    else
        return VO_ERR_NOT_IMPLEMENT;

    return VO_ERR_NONE;
}

VO_U32 VO_API voCCRRGetOutputBuffer(VO_HANDLE hCCRR)
{
    return VO_ERR_FAILED;
}

VO_U32 VO_API voCCRRGetMemoryOperator(VO_HANDLE hCCRR, VO_MEM_VIDEO_OPERATOR **ppMemOp)
{
    *ppMemOp = 0;
    return VO_ERR_FAILED;
}


VO_U32 VO_API voCCRRProcess(VO_HANDLE hCCRR, VO_VIDEO_BUFFER *pInput, VO_VIDEO_BUFFER *pOutput, VO_S64 nStart, VO_BOOL bWait)
{
    struct CC_HND *phCR = (struct CC_HND *) hCCRR;
    ClrConvData conv_data;
    //VO_S32 i = 0;
    VOCCRETURNCODE	nCcRC = VO_ERR_NONE;

#ifdef CC_LOGINFO
    VOLOGI("voCCRRProcess In");
#endif

#ifdef TIME_ANDROID
    start = GetTickCount();
#endif

    if (phCR == NULL)
        return VO_ERR_INVALID_ARG;

    if((pInput->Buffer[0] == NULL) || (pInput->Buffer[1] == NULL)
            /*|| (pInput->Buffer[2] == NULL) */ || (pOutput->Buffer[0] == NULL)
            || (phCR->conv_internal.nInWidth <= 0) || (phCR->conv_internal.nInHeight <= 0)
            || (phCR->conv_internal.nOutWidth <= 0) || (phCR->conv_internal.nOutHeight <= 0)
            || (pInput->Stride[0] <= 0) || (pInput->Stride[1] <= 0)
            /*|| (pInput->Stride[2] <= 0)*/ || (pOutput->Stride[0] <= 0))
    {
        return VO_ERR_INVALID_ARG;
    }

    //if(((VO_COLOR_YUV_420_PACK != phCR->conv_internal.nInType) && (VO_COLOR_YUV_420_PACK_2  != phCR->conv_internal.nInType)) && (pInput->Buffer[2] == NULL)){
    //	return VO_ERR_INVALID_ARG;
    //}

#ifdef CC_LOGINFO
    VOLOGI("pInput->Buffer[0] = %d, pInput->Buffer[1] = %d, pInput->Buffer[2] = %d", pInput->Buffer[0], pInput->Buffer[1], pInput->Buffer[2]);
    VOLOGI("pInput->Stride[0] = %d, pInput->Stride[1] = %d", pInput->Stride[0], pInput->Stride[1]);
    VOLOGI("pOutput->Buffer[0] = %d, pOutput->Buffer[1] = %d, pOutput->Buffer[2] = %d", pOutput->Buffer[0], pOutput->Buffer[1], pOutput->Buffer[2]);
    VOLOGI("pOutput->Stride[0] = %d, pOutput->Stride[1] = %d", pOutput->Stride[0], pOutput->Stride[1]);
#endif

    phCR->conv_internal.pInBuf[0]   = pInput->Buffer[0];
    phCR->conv_internal.pInBuf[1]   = pInput->Buffer[1];
    phCR->conv_internal.pInBuf[2]   = pInput->Buffer[2];
    phCR->conv_internal.nInStride   = pInput->Stride[0];
    phCR->conv_internal.nInUVStride = pInput->Stride[1];

    phCR->conv_internal.pOutBuf[0]  = pOutput->Buffer[0];
    phCR->conv_internal.nOutStride  = pOutput->Stride[0];

    if (VO_COLOR_YUV_PLANAR420 == phCR->conv_internal.nOutType)
    {
        phCR->conv_internal.pOutBuf[1]  = pOutput->Buffer[1];
        phCR->conv_internal.pOutBuf[2]  = pOutput->Buffer[2];
        phCR->conv_internal.nOutUVStride = pOutput->Stride[1];
    }

    conv_data = phCR->conv_internal;

    nCcRC = voCCProcess(hCCRR, (VO_PTR)&conv_data);

#ifdef CC_LOGINFO
    VOLOGI("voCCRRProcess Out");
#endif

#ifdef TIME_ANDROID
    nframes++;
    total += GetTickCount() - start;
#endif

    return nCcRC;

}

VO_U32 VO_API voCCRRWaitDone(VO_HANDLE hCCRR)
{
    return VO_ERR_NONE;
}

VO_U32 VO_API voCCRRSetCallBack(VO_HANDLE hCCRRR, VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData)
{
    return VO_ERR_NONE;
}

VO_U32 VO_API voCCRRSetParameter(VO_HANDLE hCCRRR, VO_U32 nID, VO_PTR pValue)
{

    switch(nID)
    {
    case VO_PID_CCRRR_ALPHAVALUE:
        alpha_value = (*(VO_U32 *) pValue);
        break;
    default :
        return VO_ERR_WRONG_PARAM_ID;
    }

    return VO_ERR_NONE;
}

VO_U32 VO_API voCCRRGetParameter(VO_HANDLE hCCRRR, VO_U32 nID, VO_PTR pValue)
{
    return VO_ERR_WRONG_PARAM_ID;
}

VO_U32 VO_API voCCRRUninit(VO_HANDLE hCCRR)
{
    VO_U32 return_code = VO_ERR_NONE;

#ifdef CC_LOGINFO
    VOLOGI("voCCRRUninit In");
#endif

    return_code = voCCUninit(hCCRR);

#ifdef CC_LOGINFO
    VOLOGI("voCCRRUninit Out");
#endif

#ifdef TIME_ANDROID
    LOGI("nframes = %d total = %d ms\n", nframes, total);
    printf("nframes = %d total = %d ms\n", nframes, total);
#endif

    return return_code;
}

#if defined(VOSYMBIAN)
EXPORT_C VO_S32 VO_API voGetVideoCCRRRAPI (VO_VIDEO_CCRRRAPI *pCCRRR, VO_U32 uFlag)
#else
VO_EXPORT_FUNC VO_S32 VO_API yyGetColorConvertFunc (VO_VIDEO_CCRRRAPI *pCCRRR, VO_U32 uFlag)
#endif
{
    pCCRRR->Init = voCCRRInit;
    pCCRRR->Uninit = voCCRRUninit;
    pCCRRR->GetProperty = voCCRRGetProperty;
    pCCRRR->GetInputType = voCCRRGetInputType;
    pCCRRR->GetOutputType = voCCRRGetOutputType;
    pCCRRR->SetColorType = voCCRRSetVideoType;
    pCCRRR->SetCCRRSize = voCCRRSetVideoSize;
    pCCRRR->Process = voCCRRProcess;
    pCCRRR->WaitDone = voCCRRWaitDone;
    pCCRRR->SetCallBack = voCCRRSetCallBack;
    pCCRRR->GetVideoMemOP = voCCRRGetMemoryOperator;
    pCCRRR->SetParam = voCCRRSetParameter;
    pCCRRR->GetParam = voCCRRGetParameter;

    return VO_ERR_NONE;
}
#endif //#ifndef COLCONVENABLE
