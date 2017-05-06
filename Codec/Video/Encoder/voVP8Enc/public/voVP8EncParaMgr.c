/*++

Copyright (c) 2012  VisualOn, Inc.

Module Name:

    voVP8EncParaMgr.c

Abstract:

    Implementation of VP8 encoder configuration parameter management.

Author:

    Rayman Lee    [li_chenguang@visualon.com]   6-7-2012

Revision History:

--*/

#include "voVP8EncInternal.h"
#include "vpx/vp8cx.h"

VO_S32
VP8EncSetTokenPartitions(
    VO_HANDLE hCodec, 
    VO_S32 Partition
    )
{
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal = (PVP8_ENCODER_GLOBAL_HANDLE)hCodec;
    vpx_codec_err_t Error;

    if (Partition < 0)
        return VO_ERR_INVALID_ARG;

    Error = vpx_codec_control( &EncGlobal->Context, 
                               VP8E_SET_TOKEN_PARTITIONS, 
                               (vp8e_token_partitions)Partition );
    if (VPX_CODEC_OK != Error) {

        fprintf(stderr, "Failed to set token partitions: %s\n",
                vpx_codec_error_detail(&EncGlobal->Context));

        return VO_ERR_ENC_VP8_CONFIG;
    }

    return VO_ERR_NONE;
}

VO_S32
VP8EncSetAutoAltRef( 
    VO_HANDLE hCodec, 
    VO_S32 AutoAltRef
    )
{
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal = (PVP8_ENCODER_GLOBAL_HANDLE)hCodec;
    vpx_codec_err_t Error;

    if (AutoAltRef < 0)
        return VO_ERR_INVALID_ARG;

    Error = vpx_codec_control( &EncGlobal->Context, 
                               VP8E_SET_ENABLEAUTOALTREF, 
                               AutoAltRef );
    if (VPX_CODEC_OK != Error) {

        fprintf(stderr, "Failed to set auto alt ref: %s\n",
                vpx_codec_error_detail(&EncGlobal->Context));

        return VO_ERR_ENC_VP8_CONFIG;
    }

    return VO_ERR_NONE;
}

VO_S32
VP8EncSetARNRMaxFrames(
    VO_HANDLE hCodec,
    VO_S32 ArnrMaxFrames
    )
{
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal = (PVP8_ENCODER_GLOBAL_HANDLE)hCodec;
    vpx_codec_err_t Error;

    if (ArnrMaxFrames < 0)
        return VO_ERR_INVALID_ARG;

    Error = vpx_codec_control( &EncGlobal->Context, 
                               VP8E_SET_ARNR_MAXFRAMES, 
                               ArnrMaxFrames );
    if (VPX_CODEC_OK != Error) {

        fprintf(stderr, "Failed to set arnr max frames: %s\n",
                vpx_codec_error_detail(&EncGlobal->Context));

        return VO_ERR_ENC_VP8_CONFIG;
    }

    return VO_ERR_NONE;
}

VO_S32
VP8EncSetARNRStrength(
    VO_HANDLE hCodec,
    VO_S32 ArnrStrength
    )
{
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal = (PVP8_ENCODER_GLOBAL_HANDLE)hCodec;
    vpx_codec_err_t Error;

    if (ArnrStrength < 0)
        return VO_ERR_INVALID_ARG;

    Error = vpx_codec_control( &EncGlobal->Context, 
                               VP8E_SET_ARNR_STRENGTH, 
                               ArnrStrength );
    if (VPX_CODEC_OK != Error) {

        fprintf(stderr, "Failed to set arnr strength: %s\n",
                vpx_codec_error_detail(&EncGlobal->Context));

        return VO_ERR_ENC_VP8_CONFIG;
    }

    return VO_ERR_NONE;
}

VO_S32
VP8EncSetARNRType(
    VO_HANDLE hCodec, 
    VO_S32 ArnrType
    )
{
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal = (PVP8_ENCODER_GLOBAL_HANDLE)hCodec;
    vpx_codec_err_t Error;

    if (ArnrType < 1)
        return VO_ERR_INVALID_ARG;

    Error = vpx_codec_control( &EncGlobal->Context, 
                               VP8E_SET_ARNR_TYPE, 
                               ArnrType );
    if (VPX_CODEC_OK != Error) {

        fprintf(stderr, "Failed to set arnr type: %s\n",
                vpx_codec_error_detail(&EncGlobal->Context));

        return VO_ERR_ENC_VP8_CONFIG;
    }

    return VO_ERR_NONE;
}

VO_S32
VP8EncSetCPUUsed(
    VO_HANDLE hCodec,
    VO_S32 CpuUsed
    )
{
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal = (PVP8_ENCODER_GLOBAL_HANDLE)hCodec;
    vpx_codec_err_t Error;

    if (CpuUsed < -16 || CpuUsed > 16)
        return VO_ERR_INVALID_ARG;

    Error = vpx_codec_control( &EncGlobal->Context, 
                               VP8E_SET_CPUUSED, 
                               CpuUsed );
    if (VPX_CODEC_OK != Error) {

        fprintf(stderr, "Failed to set cpu used: %s\n",
                vpx_codec_error_detail(&EncGlobal->Context));

        return VO_ERR_ENC_VP8_CONFIG;
    }

    return VO_ERR_NONE;
}

VO_S32
VP8EncSetStaticThreshold(
    VO_HANDLE hCodec,
    VO_S32 StaticThreshold
    )
{
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal = (PVP8_ENCODER_GLOBAL_HANDLE)hCodec;
    vpx_codec_err_t Error;

    if (StaticThreshold < 0)
        return VO_ERR_INVALID_ARG;

    Error = vpx_codec_control( &EncGlobal->Context, 
                               VP8E_SET_STATIC_THRESHOLD, 
                               StaticThreshold );
    if (VPX_CODEC_OK != Error) {

        fprintf(stderr, "Failed to set static threshold: %s\n",
                vpx_codec_error_detail(&EncGlobal->Context));

        return VO_ERR_ENC_VP8_CONFIG;
    }

    return VO_ERR_NONE;
}

VO_S32
VP8EncSetQuality(
    VO_HANDLE hCodec,
    VO_VP8_ENCODE_QUALITY EncodeQuality
    )
{
    PVP8_ENCODER_GLOBAL_HANDLE CodecHandle = (PVP8_ENCODER_GLOBAL_HANDLE)hCodec;
    int* Deadline = &(CodecHandle->UserConfig.Deadline);

    if (EncodeQuality == RealtimeQuality) {
        *Deadline = VPX_DL_REALTIME;
    } 
    else if (EncodeQuality == GoodQuality) {
        *Deadline = VPX_DL_GOOD_QUALITY;
    }
    else {
        *Deadline = VPX_DL_BEST_QUALITY;
    }

    return VO_ERR_NONE;
}