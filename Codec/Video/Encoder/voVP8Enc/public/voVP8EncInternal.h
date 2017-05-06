/*++ // Increment this if a change has global effects

Copyright (c) 2012  VisualOn, Inc.

Module Name:

    voVP8EncInternal.h

Abstract:

    This module defines some internal methods and data structure used to
    implement VisualOn's Media Framework API. Relevant data structure and
    actual process flow is opaque to user.

Author:

    Rayman Lee    [li_chenguang@visualon.com]   6-8-2012

Revision History:

--*/

#ifndef __VO_VP8ENC_INTERNAL_H__
#define __VO_VP8ENC_INTERNAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "vpx_config.h"
#include "voVP8.h"
#include "vpx/vpx_codec.h"
#include "vpx/vpx_encoder.h"
#include "vpx/vp8cx.h"
#include "vpx_ports/vpx_timer.h"


#define VO_DEBUG_PSNR         1


typedef struct _VP8_CODEC_ENCODER_CONFIG {

    VO_S32 Width;                       // must
    VO_S32 Height;                      // must

    VO_S32 Deadline;                    // must --rt --good or --best influence on encode speed.
    VO_S32 Profile;

    VO_S32 Threads;
    VO_S32 ErrorRresilient;
    VO_S32 LagInFrames;
    VO_S32 DropframeThresh;
    VO_S32 ResizeAllowed;
    VO_S32 ResizeUpThresh;
    VO_S32 ResizeDownThresh;
    VO_S32 EndUsage;                    //      vbr cbr or cq
    VO_S32 TargetBitrate;               // must
    VO_S32 TargetFramerate;             // must
    VO_S32 MinQuantizer;
    VO_S32 MaxQuantizer;
    VO_S32 UndershootPct;
    VO_S32 OvershootPct;
    VO_S32 DecoderBufferSize;
    VO_S32 DecoderBufferInitialSize;
    VO_S32 DecoderBufferOptimalSize;
    VO_S32 KeyframeMode;
    VO_S32 KeyframeMinInterval;
    VO_S32 KeyframeMaxInterval;
    VO_S32 PassMode;
    VO_PBYTE TwoPassStatsBuf;
    VO_S64 TwoPassStatsBuflen;
    VO_S32 TwoPassVbrBiasPct;
    VO_S32 TwoPassVbrMinsectionPct;
    VO_S32 TwoPassVbrMaxsectionPct;

    //
    // Following members is using another kind of control configuration.
    // Cannot work via directly setting value to codec config data.
    //
    VO_S32 CpuUsed;
    VO_S32 AutoAltRef;
    VO_S32 NoiseSensitivity;
    VO_S32 Sharpness;
    VO_S32 StaticThresh;
    VO_S32 TokenPartitions;
    VO_S32 ARNRMaxFrames;
    VO_S32 ARNRStrength;
    VO_S32 ARNRType;
    VO_S32 TuneSsim;
    VO_S32 CQLevel;
    VO_S32 MaxIntralRatePct;
} VP8_CODEC_ENCODER_CONFIG, *PVP8_CODEC_ENCODER_CONFIG;


#define FLAG_RAISE_WIDTH                  (1 << 1)
#define FLAG_RAISE_HEIGHT                 (1 << 2)
#define FLAG_RAISE_BITRATE                (1 << 3)
#define FLAG_RAISE_FRAMERATE              (1 << 4)
#define FLAG_RAISE_PROFILE                (1 << 5)
#define FLAG_RAISE_QUALITY                (1 << 6)


//
// This data structure is the actual data type of codec handle, and normally the 
// detail of its member is opaque to user. the most important thing is user cannot 
// directly modify the data member outside ViusalOn's API.
//

typedef struct _VP8_ENCODER_GLOBAL_HANDLE {

    vpx_codec_ctx_t Context;

    vpx_image_t Image;

    vpx_codec_iface_t* Iface;

    vpx_codec_enc_cfg_t InnerConfig;

    //
    // Use this to set internal config.
    //
    VP8_CODEC_ENCODER_CONFIG UserConfig;

    VO_VP8_ENCODER_STATUS Status;

    VO_S32 FrameCount;

    VO_S32 TimeElapse;

    VO_S64 Flag;

#if VO_DEBUG_PSNR
    VO_U64 PsnrSseTotal;

    VO_U64 PsnrSamplesTotal;

    double PsnrTotals[4];

    VO_S32 PsnrCount;
#endif

}VP8_ENCODER_GLOBAL_HANDLE, *PVP8_ENCODER_GLOBAL_HANDLE;


//
// External public function statement for linking outside.
//

extern VO_S32
VP8EncSetTokenPartitions(
    VO_HANDLE hCodec, 
    VO_S32 Partition
    );

extern VO_S32
VP8EncSetAutoAltRef( 
    VO_HANDLE hCodec, 
    VO_S32 AutoAltRef
    );

extern VO_S32
VP8EncSetARNRMaxFrames(
    VO_HANDLE hCodec,
    VO_S32 ArnrMaxFrames
    );

extern VO_S32
VP8EncSetARNRStrength(
    VO_HANDLE hCodec,
    VO_S32 ArnrStrength
    );

extern VO_S32
VP8EncSetARNRType(
    VO_HANDLE hCodec, 
    VO_S32 ArnrType
    );

extern VO_S32
VP8EncSetCPUUsed(
    VO_HANDLE hCodec,
    VO_S32 CpuUsed
    );

extern VO_S32
VP8EncSetStaticThreshold(
    VO_HANDLE hCodec,
    VO_S32 StaticThreshold
    );

extern VO_S32
VP8EncSetQuality(
    VO_HANDLE hCodec,
    VO_VP8_ENCODE_QUALITY EncodeQuality
    );

#endif // __VO_VP8ENC_INTERNAL_H__