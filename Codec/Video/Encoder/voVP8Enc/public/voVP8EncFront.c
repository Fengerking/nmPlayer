/*++

Copyright (c) 2012  VisualOn, Inc.

Module Name:

    voVP8EncFront.c

Abstract:

    Implementation of VisualOn's VP8 encoder interface functions

Author:

    Rayman Lee    [li_chenguang@visualon.com]   6-5-2012

Revision History:

--*/

#include "voVP8EncInternal.h"

//
// This macro set single flag bit value to long flag value in some specified
// bit position.
//

#define SetFlag(Flags, Value)      ( Flags |= Value )

//
// This macro returns TRUE if a flag in a set of flags is on and FALSE
// otherwise
//

#define FlagOn(Flags, SingleFlag)  ((Flags) & (SingleFlag))

//
// This macro checks whether a pointer is NULL
//

#define ValidatePointer(Pointer) {  \
    if (NULL == Pointer)            \
        return VO_ERR_INVALID_ARG;  \
}


extern void 
CopyPlane( 
    VO_U8 *dst, 
    VO_S32 i_dst,
    VO_U8 *src, 
    VO_S32 i_src, 
    VO_S32 w, 
    VO_S32 h
    );

extern void 
yuv420pack_to_yuv420_c(
    VO_U8 * y_dst, 
    VO_U8 * u_dst, 
    VO_U8 * v_dst,
    VO_S32 y_dst_stride, 
    VO_S32 uv_dst_stride,
    VO_U8 * y_src, 
    VO_U8 * uv_src,
    VO_S32 y_src_stride, 
    VO_S32 uv_src_stride,
    VO_S32 width, 
    VO_S32 height
    );

extern void uv420pack_to_uv420_c(
    VO_U8 * u_dst, 
    VO_U8 * v_dst,
    VO_S32 uv_dst_stride, 
    VO_U8 * uv_src,
    VO_S32 uv_src_stride,
    VO_S32 width, 
    VO_S32 height
    );

extern void uyvy422pack_to_uv420_c(
    VO_U8 * y_dst,
    VO_S32 y_dst_stride,
    VO_U8 * u_dst, 
    VO_U8 * v_dst,
    VO_S32 uv_dst_stride, 
    VO_U8 * src,
    VO_S32 src_stride,
    VO_S32 width, 
    VO_S32 height
    );

VO_S32
VP8EncInitializeGlobalContext(
    VO_HANDLE* Handle
    );

VO_S32 
VP8EncInitializeConfigContext (
    PVP8_CODEC_ENCODER_CONFIG UserConfig
    );

VO_S32 
VP8EncCheckBasicEnvironment (
    VO_S64 FlagStatus
    );

VO_S32 
VP8EncApplySettingContext (
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal
    );

VO_S32
VP8EncPreprocessPicture (
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal,
    vpx_image_t* Image,
    VO_VIDEO_BUFFER* Input
    );

VO_S32
VP8EncGetTimeAndDuration (
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal, 
    VO_S64* Time,
    VO_S32* Duration
    );


VO_U32 
VO_API VP8EncInitialize(
    VO_HANDLE* Handle,
    VO_VIDEO_CODINGTYPE Type, 
    VO_CODEC_INIT_USERDATA* UserData
    )
/*++

Routine Description:

    Attempt to initializer the VP8 encoder module. Global encoder context will
    be allocated here and be stored. NOTE: cannot duplicate invoking this because
    routine will try to allocate handle memory every time. MUST release it before 
    initialize again.

Arguments:

    phEnc - Return the video encoder handle.

    vType - The encoder type if the module support multi codec.

    pUserData - The init param. It is memory operator or allocated memory.

Return Value:

    VO_ERR_NONE - Routine process OK.

    VO_ERR_INVALID_ARG - Input handle is invalid.

--*/
{
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal = NULL;
    VO_LIB_OPERATOR* LibOperator = NULL;
    VO_PTR LicenseCheck = NULL;
    vpx_codec_err_t Error;

    //
    // Codec type check.
    //

    if(VO_VIDEO_CodingVP8 != Type) {
        return VO_ERR_NOT_IMPLEMENT;
    }

    //
    // Initialize global codec handle context.
    //

    if ( VP8EncInitializeGlobalContext(Handle) )
        return VO_ERR_INVALID_ARG;

    EncGlobal = (PVP8_ENCODER_GLOBAL_HANDLE)(*Handle);

    //
    // Populate default encoder configuration.
    //

    Error = vpx_codec_enc_config_default( EncGlobal->Iface, &EncGlobal->InnerConfig, 0 );
    if (VPX_CODEC_OK != Error) {

        fprintf(stderr, "Failed to get config: %s\n", vpx_codec_err_to_string(Error));
        return VO_ERR_WRONG_STATUS;
    }

    //
    // The config scales to milliseconds because the encoder will generate
    // altref frames that is one timestamp tick different than another frame.
    // The default encoder has a resolution of milliseconds so set the encoder 
    // timebase to milliseconds.
    //

    EncGlobal->InnerConfig.g_timebase.num = 1;
    EncGlobal->InnerConfig.g_timebase.den = 1000;  // millisecond ticks

    //
    // After initializing global context and using default codec interface to 
    // obtain internal config, we consider that initialize process OK, hence, 
    // we change status to wait.
    //

    EncGlobal->Status = VP8EncStatusWait;
    
    return VO_ERR_NONE;
}


VO_U32  
VO_API VP8EncUninitialize(
    VO_HANDLE Handle
    )
/*++

Routine Description:

    Attempt to close the VP8 encoder instance, release any resource it used.

Arguments:

    Handle - VP8 encoder instance handle, returned by voVP8EncInit().

Return Value:

    VO_ERR_NONE - Routine process OK.

    VO_ERR_INVALID_ARG - Input handle is invalid.

--*/
{
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal = NULL;
    vpx_codec_ctx_t* Context = NULL;
    vpx_image_t* Image = NULL;

    ValidatePointer(Handle);

    EncGlobal = (PVP8_ENCODER_GLOBAL_HANDLE)Handle;

    //
    // Show final statistics encoder result.
    //
    fprintf(stderr, "\rProcess Frame %4d %7lu %s (%.2f fps)\033[K",
            EncGlobal->FrameCount,
            EncGlobal->TimeElapse > 9999999 ? EncGlobal->TimeElapse / 1000 : EncGlobal->TimeElapse,
            EncGlobal->TimeElapse > 9999999 ? "ms" : "us",
            EncGlobal->TimeElapse > 0 ? (float)EncGlobal->FrameCount * 1000000.0f / EncGlobal->TimeElapse : 0);

    Context = &(EncGlobal->Context);
    Image = &(EncGlobal->Image);

    vpx_codec_destroy(Context);

    vpx_img_free(Image);

    free((VO_VOID *)Handle);

    return VO_ERR_NONE;
}

VO_U32 
VO_API VP8EncSetParameter(
    VO_HANDLE Handle, 
    VO_S32 ParamID, 
    VO_PTR Data
    )
/*++

Routine Description:

    This routine is used to get parameter of the encoder instance.

Arguments:

    Handle - VP8 encoder instance handle, returned by voVP8EncInit().

    ParamID - Parameter ID, Supports: VOVP8PARAMETERID.

    Data - Parameter value. The value may be a pointer to a struct 
        according to the parameter ID.

Return Value:

    VO_ERR_NONE - Routine process OK.

    VO_ERR_WRONG_PARAM_ID - No such parameter ID supported.

--*/
{
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal = NULL;
    PVP8_CODEC_ENCODER_CONFIG UserConfig = NULL;

    ValidatePointer(Handle);
    ValidatePointer(Data);

    EncGlobal = (PVP8_ENCODER_GLOBAL_HANDLE)Handle;
    UserConfig = &EncGlobal->UserConfig;

    //
    // Check internal global context status.
    //

    if (EncGlobal->Status == VP8EncStatusUnknown) {

        fprintf(stderr, "Handle(0x%x): Encoder has not been properly initialized!\n", Handle);
        goto ErrorReturn;
    } 
    else if (EncGlobal->Status == VP8EncStatusReady) {

        fprintf(stderr, "Handle(0x%x): You have make encoder ready to process!\n", Handle);
        goto ErrorReturn;
    }
    else if (EncGlobal->Status == VP8EncStatusEncode) {

        // This should not happened in normal case.
        assert(0);
        goto ErrorReturn;
    }

    
    //
    // Parse different kinds of encoder configuration.
    //

    switch (ParamID) {

    case VO_PID_ENC_VP8_BITRATE:
        UserConfig->TargetBitrate = *((VO_S32*)Data);
        SetFlag(EncGlobal->Flag, FLAG_RAISE_BITRATE);
        break;

    case VO_PID_ENC_VP8_WIDTH:
        UserConfig->Width = *((VO_S32*)Data);
        SetFlag(EncGlobal->Flag, FLAG_RAISE_WIDTH);
        break;

    case VO_PID_ENC_VP8_HEIGHT:
        UserConfig->Height = *((VO_S32*)Data);
        SetFlag(EncGlobal->Flag, FLAG_RAISE_HEIGHT);
        break;

    case VO_PID_ENC_VP8_FRAMERATE:
        UserConfig->TargetFramerate = *((VO_S32*)Data);
        SetFlag(EncGlobal->Flag, FLAG_RAISE_FRAMERATE);
        break;

    case VO_PID_ENC_VP8_VIDEO_QUALITY:
        VP8EncSetQuality(Handle, *((VO_S32*)Data));
        SetFlag(EncGlobal->Flag, FLAG_RAISE_QUALITY);
        break;

    case VO_PID_ENC_VP8_PROFILE:
        UserConfig->Profile = *((VO_S32*)Data);
        SetFlag(EncGlobal->Flag, FLAG_RAISE_PROFILE);
        break;

    default:
        return VO_ERR_WRONG_PARAM_ID;
    }

    //
    // By setting parameter indicates we are now setting codec, marking current
    // status is initializing.
    //

    EncGlobal->Status = VP8EncStatusPrepare;
    return VO_ERR_NONE;

ErrorReturn:
    return VO_ERR_FAILED;
}

VO_U32 
VO_API VP8EncGetParameter(
	VO_HANDLE Handle, 
	VO_S32 ParamID, 
	VO_PTR Data
	)
/*++

Routine Description:

    This routine is used to set parameter of the encoder instance.

Arguments:

    Handle - VP8 encoder instance handle, returned by voVP8EncInit().

    ParamID - Parameter ID, Supports: VOVP8PARAMETERID.

    Data - Parameter value. The value may be a pointer to a struct 
        according to the parameter ID.

Return Value:

    VO_ERR_NONE - Routine process OK.

    VO_ERR_INVALID_ARG - No such parameter ID supported.

--*/
{
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal = NULL;
    VO_S32 UsePsnr = 0;
    vpx_codec_err_t Error;

    ValidatePointer(Handle);
    ValidatePointer(Data);

    EncGlobal = (PVP8_ENCODER_GLOBAL_HANDLE)Handle;

    switch (ParamID) {

    case VO_PID_ENC_VP8_ENCODER_STATUS:
    {
        if (EncGlobal->Status == VP8EncStatusReady) {
            // We have check encode environment, this will not be handled again.
            return VO_ERR_NONE;
        }
        else if (EncGlobal->Status == VP8EncStatusWait || EncGlobal->Status == VP8EncStatusUnknown) {
            // We have not set any parameters to encoder.
            return VO_ERR_ENC_VP8_CONFIG;
        }
        else if (EncGlobal->Status == VP8EncStatusEncode) {
            // ??? for multi-thread check the same handle, valid ???
            assert(0);
            return VO_ERR_FAILED;
        }
        
        //
        // Check whether we have set enough parameter to initialize basic encoder configuration.
        //

        if ( VP8EncCheckBasicEnvironment(EncGlobal->Flag) ) {

            *((VO_S32 *)Data) = EncGlobal->Status;
            fprintf(stderr, "No enough necessary parameters to init encoder!\n");
            return VO_ERR_ENC_VP8_NO_ENOUGH_PARA;
        }

        //
        // Here if configuration meets minimal encoder requirements, we set status ready to
        // tell the user that codec context is now being ready to encode.
        //

        EncGlobal->Status = VP8EncStatusReady;

        //
        // User config will be applied to internal vpx_codec_enc_cfg_t, at this stage, most of
        // parameter cannot be changed any more.
        //

        VP8EncApplySettingContext(EncGlobal);

        //
        // Construct encoder context using default inner config. This means that we have
        // made a valid encoder context.
        //

#if VO_DEBUG_PSNR
        UsePsnr = 1;
#endif
        Error = vpx_codec_enc_init( &EncGlobal->Context, 
                                    EncGlobal->Iface, 
                                    &EncGlobal->InnerConfig, 
                                    UsePsnr > 0 ? VPX_CODEC_USE_PSNR : 0 );
        if (VPX_CODEC_OK != Error) {

            fprintf(stderr, "Failed to initialize encoder: %s\n", vpx_codec_err_to_string(Error));
            return VO_ERR_FAILED;
        }

        //
        // Allocate VP8 sdk's internal raw image data structure.
        //

        vpx_img_alloc( &EncGlobal->Image, 
                       VPX_IMG_FMT_I420, 
                       EncGlobal->UserConfig.Width, 
                       EncGlobal->UserConfig.Height, 
                       1 );

        *((VO_S32 *)Data) = EncGlobal->Status;
    }
        break;

    case VO_PID_ENC_VP8_OUTBUF_SIZE:

        *((VO_S32 *)Data) = (EncGlobal->UserConfig.Width * EncGlobal->UserConfig.Height) * 3/2;
        break;

    default:
		
        return VO_ERR_WRONG_PARAM_ID;
    }

    return VO_ERR_NONE;
}

VO_S32 
VO_API VP8EncProcess(
    VO_HANDLE Handle, 
    VO_VIDEO_BUFFER * Input, 
    VO_CODECBUFFER * Output, 
    VO_VIDEO_FRAMETYPE * Type
    )
/*++

Routine Description:

    This routine performs an execution of encoding a frame data.

Arguments:

    Handle - VP8 Encoder instance handle, returned by voVP8EncInit().

    Input - Input buffer pointer. 

    Output - Output buffer pointer and length. pOutData->buffer should be alloced 
        by Application, buffer size can get from VP8EncGetParameter(), ID is 
        VO_PID_ENC_VP8_OUTBUF_SIZE. When the function return, pOutData->length indicates the 
        encoded stream size.

    Type - Output encoded frame type.

Return Value:

    VO_ERR_NONE - Routine process OK.

    VO_ERR_WRONG_STATUS - The Encoder is not ready to encode. Usually this indicates more 
        parameters needed. See VP8EncSetParameter().

--*/
{
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal = NULL;
    vpx_codec_iter_t Iterator = NULL;
    const vpx_codec_cx_pkt_t* Packet;
    vpx_codec_err_t Error;
    struct vpx_usec_timer Timer;
    VO_S64 Pts;
    VO_U32 Duration = 0, LoopCount = 0, InputAvailable = 0;
    VO_S32 GetData = 0;
    VO_PBYTE WriteBuffer = NULL;

    ValidatePointer(Handle);
    ValidatePointer(Output);

    EncGlobal = (PVP8_ENCODER_GLOBAL_HANDLE)Handle;

    //
    // Check internal context status, if not in good condition, just return.
    //

    if (EncGlobal->Status != VP8EncStatusReady) {

        if(EncGlobal->Status == VP8EncStatusPrepare) {
            
            if (VP8EncCheckBasicEnvironment(EncGlobal->Flag)) {
                return VO_ERR_ENC_STATUS_UNCHECKED;
            } else {
                fprintf(stderr, "Handle(0x%x): There is no enough parameter to start encode!\n", Handle);
                return VO_ERR_ENC_VP8_NO_ENOUGH_PARA;
            }
        }
        else if(EncGlobal->Status == VP8EncStatusWait || 
                EncGlobal->Status == VP8EncStatusUnknown) {

            fprintf(stderr, "Handle(0x%x): No parameter set, have you properly init or set?\n", Handle);
            return VO_ERR_ENC_VP8_CONFIG;
        }
        else {
            assert(0); // should this really happened ?
            return VO_ERR_FAILED;
        }
    }
    
    //
    // Process encode, we set status to encode.
    //

    EncGlobal->Status = VP8EncStatusEncode;

    //
    // Calculate frame pts and playback duration using internal mechanism.
    //

    vpx_usec_timer_start(&Timer);

    if( VP8EncGetTimeAndDuration( EncGlobal, &Pts, &Duration ) )
        return VO_ERR_INVALID_ARG;

    if (Input) {

        InputAvailable = 1;

        //
        // Copy to vpx_image_t internal extended frame buffer from input.
        //
        if( VP8EncPreprocessPicture(EncGlobal, &EncGlobal->Image, Input) )
            return VO_ERR_INVALID_ARG;
    }

    EncGlobal->FrameCount++;
    fprintf(stderr, "\rFrame %4d ", EncGlobal->FrameCount);

    //
    // Encode raw frame to VP8 bitstream packet. NOTE: null input will result in internal Image null.
    //

    vpx_codec_encode( &EncGlobal->Context, 
                      InputAvailable ? &EncGlobal->Image : NULL, 
                      Pts, 
                      Duration, 
                      0, 
                      EncGlobal->UserConfig.Deadline );

    vpx_usec_timer_mark(&Timer);
    EncGlobal->TimeElapse += vpx_usec_timer_elapsed(&Timer);

    //
    // Reset output buffer's length and pts.
    //
    Output->Length = 0;
    Output->Time = 0;

    //
    // Get encoded compressed bitstream packet.
    //
    while ( (Packet = vpx_codec_get_cx_data(&EncGlobal->Context, &Iterator)) ) {

        switch (Packet->kind) {

        case VPX_CODEC_CX_FRAME_PKT:

            assert(Output->Buffer != NULL);

            //
            // Get output buffer address for memory writing.
            //
            WriteBuffer = Output->Buffer;

            //
            // We have just get a valid data packet.
            //
            GetData = 1;

            //
            // Now copy a valid data packet for output parameter. If we enable multi-
            // partitions mode, the packet only contains a partial slice data. So we need
            // to get multi-slice to user as a whole frame. need to copy many times. 
            // NOTE: Output buffer should have enough space.
            // If we enable multi-token partitions, vpx_codec_get_cx_data will add more
            // than one packet to list. That means one input can produce more packets. In
            // this method, the phenomenon will break the balance of input and output rate.
            //

            memcpy(WriteBuffer, Packet->data.frame.buf, Packet->data.frame.sz);
            WriteBuffer += Packet->data.frame.sz;
            Output->Length += Packet->data.frame.sz;
            Output->Time = Packet->data.frame.pts;

            //
            // Check output frame packet type.
            //
            if (Type) {

                *Type = FlagOn(Packet->data.frame.flags, VPX_FRAME_IS_KEY) ? VO_VIDEO_FRAME_I : VO_VIDEO_FRAME_P;
            }

            fprintf(stderr, " %6luF", (unsigned long)Output->Length);

            break;

        case VPX_CODEC_PSNR_PKT:

#if VO_DEBUG_PSNR

            EncGlobal->PsnrCount++;
            EncGlobal->PsnrSseTotal += Packet->data.psnr.sse[0];
            EncGlobal->PsnrSamplesTotal += Packet->data.psnr.samples[0];

            for (LoopCount = 0; LoopCount < 4; LoopCount++) {

                fprintf(stderr, "%.3lf ", Packet->data.psnr.psnr[LoopCount]);
                EncGlobal->PsnrTotals[LoopCount] += Packet->data.psnr.psnr[LoopCount];
            }
#endif
            break;

        case VPX_CODEC_STATS_PKT:

            break;

        default:

            break;
        }

        fflush(stdout);
    }

    EncGlobal->Status = VP8EncStatusReady;

    return GetData ? VO_ERR_NONE : VO_ERR_ENC_VP8_PACKET_NOT_READY;
}

VO_S32
VP8EncInitializeGlobalContext(
    VO_HANDLE* Handle
    )
/*++

Routine Description:

    This routine allocate internal codec handle with relevant global memory.

Arguments:

    None.

Return Value:

    EncGlobal - handle pointer while allocating memory process OK.

    NULL - Routine encounter an out of memory error.

--*/
{
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal = NULL;
    VO_U32 LoopCount = 0;

    assert(NULL != Handle);

    EncGlobal = (PVP8_ENCODER_GLOBAL_HANDLE)malloc(sizeof(VP8_ENCODER_GLOBAL_HANDLE));
    if (!EncGlobal) {
        return VO_ERR_OUTOF_MEMORY;
    }

    //
    // Initialize global user config context.
    //

    VP8EncInitializeConfigContext( &EncGlobal->UserConfig);

    //
    // Obtain the SDK's pre-defined default codec interface with configuration.
    //

    EncGlobal->Iface = vpx_codec_vp8_cx();

    //
    // Reset parameter status flags, indicating we have set nothing.
    //

    EncGlobal->Flag = 0;

    //
    // Count of frames initialize to zero.
    //

    EncGlobal->FrameCount = 0;

    EncGlobal->TimeElapse = 0;

    //
    // At the stage of allocating global context, status reset.
    //

    EncGlobal->Status = VP8EncStatusUnknown;

#if VO_DEBUG_PSNR

    //
    // PSNR relevant variables reset to zero.
    //

    EncGlobal->PsnrSseTotal = 0;
    EncGlobal->PsnrSamplesTotal = 0;

    for (LoopCount = 0; LoopCount < 4; ++LoopCount) {
        EncGlobal->PsnrTotals[LoopCount] = 0;
    }

    EncGlobal->PsnrCount = 0;

#endif

    *Handle = (VO_HANDLE)EncGlobal;

    return VO_ERR_NONE;
}

VO_S32 
VP8EncInitializeConfigContext (
    PVP8_CODEC_ENCODER_CONFIG UserConfig
    )
{
    ValidatePointer(UserConfig);

    //
    // These basic parameters can be used to construct a valid encoder.
    //
    UserConfig->Width = -1;
    UserConfig->Height = -1;
    UserConfig->TargetBitrate = -1;
    UserConfig->TargetFramerate = -1;
    UserConfig->Deadline = -1;
    UserConfig->Profile = -1;
    UserConfig->Threads = -1;

    UserConfig->ErrorRresilient = -1;
    UserConfig->LagInFrames = -1;
    UserConfig->MinQuantizer = -1;
    UserConfig->MaxQuantizer = -1;
    UserConfig->UndershootPct = -1;
    UserConfig->OvershootPct = -1;
    UserConfig->DecoderBufferSize = -1;
    UserConfig->DecoderBufferInitialSize = -1;
    UserConfig->DecoderBufferOptimalSize = -1;
    UserConfig->KeyframeMode = -1;
    UserConfig->KeyframeMaxInterval = -1;
    UserConfig->KeyframeMinInterval = -1;
    UserConfig->DropframeThresh = -1;
    UserConfig->ResizeAllowed = -1;
    UserConfig->ResizeUpThresh = -1;
    UserConfig->ResizeDownThresh = -1;
    UserConfig->EndUsage = -1;
    UserConfig->PassMode = -1;
    UserConfig->TwoPassStatsBuf = 0;
    UserConfig->TwoPassStatsBuflen = -1;
    UserConfig->TwoPassVbrBiasPct = -1;
    UserConfig->TwoPassVbrMinsectionPct = -1;
    UserConfig->TwoPassVbrMaxsectionPct = -1;

    UserConfig->CpuUsed = -1;
    UserConfig->AutoAltRef = -1;
    UserConfig->NoiseSensitivity = -1;
    UserConfig->Sharpness = -1;
    UserConfig->StaticThresh = -1;
    UserConfig->TokenPartitions = -1;
    UserConfig->ARNRMaxFrames = -1;
    UserConfig->ARNRStrength = -1;
    UserConfig->ARNRType = -1;

    return VO_ERR_NONE;
}


VO_S32 
VP8EncCheckBasicEnvironment (
    VO_S64 FlagStatus
    )
{
    if( FlagOn(FlagStatus, FLAG_RAISE_WIDTH) == 0 )
        return VO_ERR_FAILED;
    if( FlagOn(FlagStatus, FLAG_RAISE_HEIGHT) == 0 )
        return VO_ERR_FAILED;
    if( FlagOn(FlagStatus, FLAG_RAISE_BITRATE) == 0 )
        return VO_ERR_FAILED;
    if( FlagOn(FlagStatus, FLAG_RAISE_FRAMERATE) == 0 )
        return VO_ERR_FAILED;
    if( FlagOn(FlagStatus, FLAG_RAISE_PROFILE) == 0 )
        return VO_ERR_FAILED;
    if( FlagOn(FlagStatus, FLAG_RAISE_QUALITY) == 0 )
        return VO_ERR_FAILED;

    return VO_ERR_NONE;
}

VO_S32 
VP8EncApplySettingContext (
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal
    )
/*++

Routine Description:

    This routine dispatch user input config to codec internal global config with
    data safety check.

Arguments:

    EncGlobal - Global encoder context handle.

Return Value:

    VO_ERR_NONE - Routine process OK.

--*/
{
    vpx_codec_enc_cfg_t* TargetConfig = &EncGlobal->InnerConfig;
    VP8_CODEC_ENCODER_CONFIG* SourceConfig = &EncGlobal->UserConfig;

    TargetConfig->g_w = SourceConfig->Width;
    TargetConfig->g_h = SourceConfig->Height;
    TargetConfig->g_profile = SourceConfig->Profile;

    if (SourceConfig->TargetBitrate >= 0)
        TargetConfig->rc_target_bitrate = SourceConfig->TargetBitrate;

    if (SourceConfig->Threads >= 0)
        TargetConfig->g_threads = SourceConfig->Threads;

    if (SourceConfig->ErrorRresilient >= 0)
        TargetConfig->g_error_resilient = SourceConfig->ErrorRresilient;

    if (SourceConfig->LagInFrames >= 0)
        TargetConfig->g_lag_in_frames = SourceConfig->LagInFrames;

    if (SourceConfig->DropframeThresh < 0)
        TargetConfig->rc_dropframe_thresh = 0; // disable by default
    else
        TargetConfig->rc_dropframe_thresh = SourceConfig->DropframeThresh;

    switch (SourceConfig->EndUsage) {

    case EndUsageVBR:
        TargetConfig->rc_end_usage = VPX_VBR;
        break;

    case EndUsageCBR:
        TargetConfig->rc_end_usage = VPX_CBR;
        break;

    default:
        break;
    }

    if (SourceConfig->MinQuantizer >= 0)
        TargetConfig->rc_min_quantizer = SourceConfig->MinQuantizer;

    if (SourceConfig->MaxQuantizer >= 0)
        TargetConfig->rc_max_quantizer = SourceConfig->MaxQuantizer;

    switch (SourceConfig->KeyframeMode) {

    case KeyframeModeDisabled:
        TargetConfig->kf_mode = VPX_KF_DISABLED;
    	break;

    case KeyframeModeAuto:
        TargetConfig->kf_mode = VPX_KF_AUTO;
        break;

    default:
        break;
    }

    if (SourceConfig->KeyframeMinInterval >= 0)
        TargetConfig->kf_min_dist = SourceConfig->KeyframeMinInterval;

    if (SourceConfig->KeyframeMaxInterval >= 0)
        TargetConfig->kf_max_dist = SourceConfig->KeyframeMaxInterval;

    return VO_ERR_NONE;
}


VO_S32
VP8EncPreprocessPicture (
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal,
    vpx_image_t* Image,
    VO_VIDEO_BUFFER* Input
    )
/*++

Routine Description:

    This routine reads the input stream from user parameter and performs an
    execution of color conversion to vpx_image_t. Any color type will be transformed
    to standard YUV420 type.

Arguments:

    EncGlobal - Global context handle.

    Image - Sink standard YUV420 destination raw buffer.

    Input - VisualOn's raw buffer structure, maybe various type.

Return Value:

    VO_ERR_NONE - Routine performs process OK.

    VO_ERR_INVALID_ARG - Routine encounter an invalid args error.

--*/
{
    VO_U32 LoopCount = 0;
    VO_PBYTE Buffer = NULL;
    VO_S32 Width, Height, Stride;

    ValidatePointer(Input->Buffer[0]);
    ValidatePointer(Input->Buffer[1]);
    ValidatePointer(Input->Buffer[2]);

    switch (Input->ColorType) {
        
    case VO_COLOR_YUV_PLANAR420:

        for (LoopCount = 0; LoopCount < 3; ++LoopCount) {

            Buffer = Input->Buffer[LoopCount];
            Stride = Input->Stride[LoopCount];
            Width = EncGlobal->InnerConfig.g_w >> !!LoopCount;
            Height = EncGlobal->InnerConfig.g_h >> !!LoopCount;

            CopyPlane( Image->planes[LoopCount], Image->stride[LoopCount], Buffer, Stride, Width, Height );
        }
        break;

    case VO_COLOR_YUV_420_PACK:

        Width = EncGlobal->InnerConfig.g_w;
        Height = EncGlobal->InnerConfig.g_h;

        CopyPlane( Image->planes[0], Image->stride[0], Buffer, Stride, Width, Height );
        uv420pack_to_uv420_c( Image->planes[1], 
                              Image->planes[2], 
                              Image->stride[1], 
                              Input->Buffer[1], 
                              Input->Stride[1], 
                              Width, 
                              Height );

        break;

    case VO_COLOR_UYVY422_PACKED:

        Width = EncGlobal->InnerConfig.g_w;
        Height = EncGlobal->InnerConfig.g_h;

        uyvy422pack_to_uv420_c( Image->planes[0], 
                                Image->stride[0], 
                                Image->planes[1], 
                                Image->planes[2], 
                                Image->stride[1], 
                                Input->Buffer[0], 
                                Input->Stride[0], 
                                Width, 
                                Height );
        break;

    case VO_COLOR_YUV_420_PACK_2:

        Buffer = Input->Buffer[0];
        Stride = Input->Stride[0];
        Width = EncGlobal->InnerConfig.g_w;
        Height = EncGlobal->InnerConfig.g_h;

        CopyPlane( Image->planes[0], Image->stride[0], Buffer, Stride, Width, Height );
        uv420pack_to_uv420_c( Image->planes[2], 
                              Image->planes[1], 
                              Image->stride[1], 
                              Input->Buffer[1], 
                              Input->Stride[1], 
                              Width, 
                              Height );

        break;
    }

    return VO_ERR_NONE;
}


VO_S32
VP8EncGetTimeAndDuration(
    PVP8_ENCODER_GLOBAL_HANDLE EncGlobal, 
    VO_S64* Time,
    VO_S32* Duration
    )
/*++

Routine Description:

    This routine is invoked to calculate each frame's pts and duration.

Arguments:

    EncGlobal - Global context handle.

    Time - Each frame's pts.

    Duration - Each frame's playback duration.

Return Value:

    VO_ERR_NONE - Routine process OK.

--*/
{
    vpx_rational_t* Timebase = & (EncGlobal->InnerConfig.g_timebase);
    VO_S32 Numerator = EncGlobal->UserConfig.TargetFramerate;
    VO_S32 Demominator = 1;

    Numerator = EncGlobal->UserConfig.TargetFramerate;

    (*Time) = (VO_S64)(Timebase->den * (VO_S64)(EncGlobal->FrameCount) * Demominator 
            / Timebase->num / Numerator);

    (*Duration) = (VO_S32)(Timebase->den * Demominator 
                 / Timebase->num / Numerator);

    return VO_ERR_NONE;
}

VO_S32 
VO_API voGetVP8EncAPI (
    VO_VIDEO_ENCAPI* EncHandle
    )

/*++

Routine Description:

    This routine is called for getting video encoder API interface.

Arguments:

    pEncHandle  -  This is the VP8 Encoder structure handle.

Return Value:

    VO_ERR_NONE - Routine process OK.

    VO_ERR_INVALID_ARG - Input handle is invalid.

--*/
{
	VO_VIDEO_ENCAPI *pVP8EncHandle = EncHandle;

	if(!pVP8EncHandle)
		return VO_ERR_INVALID_ARG;

	pVP8EncHandle->Init   = VP8EncInitialize;
	pVP8EncHandle->Uninit = VP8EncUninitialize;
	pVP8EncHandle->SetParam = VP8EncSetParameter;
	pVP8EncHandle->GetParam = VP8EncGetParameter;
	pVP8EncHandle->Process = VP8EncProcess;


	return VO_ERR_NONE;
}

