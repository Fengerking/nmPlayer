/*++

Copyright (c) 2012  VisualOn, Inc.

Module Name:

    voFrameworkTest.c

Abstract:

    Test program for VisualOn Media Engine codec relevant framework API Calls

Author:

    Rayman Lee    [li_chenguang@visualon.com]   6-11-2012

Revision History:

--*/

#if defined _MSC_VER
#include <windows.h>
#elif defined __GNUC__
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <voVP8.h>

#define fourcc 0x30385056

//
// Internal static function definitions set here.
//
static void MemPutLe16(
    VO_PBYTE Memory, 
    VO_U32 Value
    ) 
{
    Memory[0] = Value;
    Memory[1] = Value >> 8;
}

static VO_VOID MemPutLe32(
    VO_PBYTE Memory, 
    VO_U32 Value
    ) 
{
    Memory[0] = Value;
    Memory[1] = Value >> 8;
    Memory[2] = Value >> 16;
    Memory[3] = Value >> 24;
}

static VO_S32 AppendIvfPacketHeader(
    FILE* OutFile,
    VO_CODECBUFFER* EncodedBuffer
    )
{
    VO_BYTE Header[12];
    VO_S64 Pts = EncodedBuffer->Time;
    VO_S32 Length = EncodedBuffer->Length;

    MemPutLe32(Header, Length);
    MemPutLe32(Header + 4, Pts & 0xFFFFFFFF);
    MemPutLe32(Header + 8, Pts >> 32);

    Length = fwrite(Header, 1, 12, OutFile);

    return Length;
}

static VO_VOID AppendIvfFileHeader(
    FILE* Outfile,
    VO_S32 Width,
    VO_S32 Height,
    VO_S32 FrameCount
    ) 
{
    VO_BYTE Header[32];

    Header[0] = 'D';
    Header[1] = 'K';
    Header[2] = 'I';
    Header[3] = 'F';

    MemPutLe16(Header + 4,  0);                                   /* version */
    MemPutLe16(Header + 6,  32);                                  /* header size */
    MemPutLe32(Header + 8,  fourcc);                              /* header size */
    MemPutLe16(Header + 12, Width);                               /* width */
    MemPutLe16(Header + 14, Height);                              /* height */
    MemPutLe32(Header + 16, 1000/*g_timebase.den*/); /* rate */
    MemPutLe32(Header + 20, 1   /*g_timebase.num*/); /* scale */
    MemPutLe32(Header + 24, FrameCount);                          /* length */
    MemPutLe32(Header + 28, 0);                                   /* unused */

    if(fwrite(Header, 1, 32, Outfile));
}


static VO_S32 ReadFrame(
    FILE* File, 
    VO_VIDEO_BUFFER* Picture, 
    VO_S32 Width, 
    VO_S32 Height
    )
{
    VO_S32 LoopCount = 0;
    VO_S32 FrameAvailable = 0;
    VO_PBYTE BuffPtr = NULL;

    if (Picture->ColorType == VO_COLOR_YUV_PLANAR420) {

        // A whole Y plane data has been loaded.
        BuffPtr = Picture->Buffer[0];
        for (LoopCount = 0; LoopCount < Height; ++LoopCount) {
            
            FrameAvailable |= fread(BuffPtr, 1, Width, File);
            BuffPtr += Picture->Stride[0];
        }

        // A whole U plane data has been loaded.
        BuffPtr = Picture->Buffer[1];
        for (LoopCount = 0; LoopCount < (Height+1)/2; ++LoopCount) {

            FrameAvailable |= fread(BuffPtr, 1, (Width+1)/2, File);
            BuffPtr += Picture->Stride[1];
        }

        // A whole V plane data has been loaded.
        BuffPtr = Picture->Buffer[2];
        for (LoopCount = 0; LoopCount < (Height+1)/2; ++LoopCount) {

            FrameAvailable |= fread(BuffPtr, 1, (Width+1)/2, File);
            BuffPtr += Picture->Stride[2];
        }
    }
    
    return FrameAvailable? VO_ERR_NONE : VO_ERR_FAILED;
}

static VO_S32 AllocatePictureMemory(
    VO_VIDEO_BUFFER* Image,
    VO_S32 Width,
    VO_S32 Height
    )
{
    VO_S32 Loop = 0;

    assert(Width > 0 && Height > 0);

    Image->Stride[0] = Width * 5/4;
    Image->Stride[1] = (Width + 1)/2 * 5/4;
    Image->Stride[2] = Image->Stride[1];
    Image->Time = 0;
    Image->Buffer[0] = Image->Buffer[1] = Image->Buffer[2] = NULL;

    // YUV420 type process.
    if (Image->ColorType == VO_COLOR_YUV_PLANAR420) {

        Image->Buffer[0] = (VO_PBYTE)malloc(sizeof(VO_BYTE) * Image->Stride[0] * Height);
        if (NULL == Image->Buffer[0])
            goto Failure;

        // U plane allocation.
        Image->Buffer[1] = (VO_PBYTE)malloc(sizeof(VO_BYTE) * Image->Stride[1] * (Height + 1)/2);
        if (NULL == Image->Buffer[1])
            goto Failure;

        // U plane allocation.
        Image->Buffer[2] = (VO_PBYTE)malloc(sizeof(VO_BYTE) * Image->Stride[2] * (Height + 1)/2);
        if (NULL == Image->Buffer[2])
            goto Failure;

        return VO_ERR_NONE;

    }
    return VO_ERR_NOT_IMPLEMENT;

Failure:
    for (Loop = 0; Loop < 3; Loop++) {
        if (NULL != Image->Buffer[Loop]) {
            free(Image->Buffer[Loop]);
            Image->Buffer[Loop] = NULL;
        }
    }
    return VO_ERR_FAILED;
}

static VO_VOID FreePictureMemory (
    VO_VIDEO_BUFFER* Image
    )
{
    if (Image) {
    
        if (Image->Buffer[0]) {
            free(Image->Buffer[0]);
            Image->Buffer[0] = NULL;
        }

        if (Image->Buffer[1]) {
            free(Image->Buffer[1]);
            Image->Buffer[1] = NULL;
        }

        if (Image->Buffer[2]) {
            free(Image->Buffer[2]);
            Image->Buffer[2] = NULL;
        }
    }
}

static VO_VOID DisplayUsage(
    VO_BYTE** argv
    )
{
    printf("Usage: %s <width> <height> <bitrate> <framerate> <infile> <outfile>\n", argv[0]);
}

VO_S32 main(VO_S32 argc, VO_BYTE** argv) 
{
    VO_VIDEO_ENCAPI EncInstance;
    VO_HANDLE EncHandle = NULL;
    VO_CODEC_INIT_USERDATA UserData;
    VO_S32 Width = 0;
    VO_S32 Height = 0;
    VO_S32 Bitrate = 0;
    VO_S32 Framerate = 0;
    VO_S32 Profile = 0;
    VO_S32 PictureSize = 0;
    VO_S32 Result = 0, FrameAvailable = 0;
    VO_VP8_ENCODE_QUALITY Quality = RealtimeQuality;
    VO_VIDEO_BUFFER PictureBuffer;
    VO_CODECBUFFER EncodedBuffer;
    VO_VP8_ENCODER_STATUS Status;
    FILE *InFileStream, *OutFileStream;

    if (argc < 7) {
        
        DisplayUsage(argv);
        return VO_ERR_NOT_IMPLEMENT;
    }
    
    //
    // Parse user input parameter.
    //

    Width = strtol(argv[1], NULL, 0);
    Height = strtol(argv[2], NULL, 0);
    Bitrate = strtol(argv[3], NULL, 0);
    Framerate = strtol(argv[4], NULL, 0);

    if((InFileStream = fopen(argv[5], "rb")) == NULL)
        return VO_ERR_INVALID_ARG;
    if ((OutFileStream = fopen(argv[6], "wb")) == NULL)
        return VO_ERR_INVALID_ARG;

    //
    // Get VisualOn's Encoder API interface context.
    //

    Result = voGetVP8EncAPI(&EncInstance);
    if (Result != VO_ERR_NONE)
        return VO_ERR_RETRY;

    //
    // Initialize encoder handle. query internal codec handle.
    //

    EncInstance.Init(&EncHandle, VO_VIDEO_CodingVP8, NULL);

    //
    // Set necessary encoder configuration parameters. If there is not enough parameters,
    // internal codec context instance cannot be initialized.
    //

    EncInstance.SetParam(EncHandle, VO_PID_ENC_VP8_WIDTH, &Width);
    EncInstance.SetParam(EncHandle, VO_PID_ENC_VP8_HEIGHT, &Height);
    EncInstance.SetParam(EncHandle, VO_PID_ENC_VP8_BITRATE, &Bitrate);
    EncInstance.SetParam(EncHandle, VO_PID_ENC_VP8_FRAMERATE, &Framerate);
    EncInstance.SetParam(EncHandle, VO_PID_ENC_VP8_VIDEO_QUALITY, &Quality);
    EncInstance.SetParam(EncHandle, VO_PID_ENC_VP8_PROFILE, &Profile);

    //
    // After setting enough various types of encoding parameters, we shall check the encoder
    // status, find out whether we can encode now.
    //
    EncInstance.GetParam(EncHandle, VO_PID_ENC_VP8_ENCODER_STATUS, &Status);
    if (Status != VP8EncStatusReady) {
        fprintf(stderr, "encoder status is not ready now!\n");
        return VO_ERR_NOT_IMPLEMENT;
    }

    AppendIvfFileHeader(OutFileStream, Width, Height, 0);

    //
    // Get output encoded sink data packet size for pre-allocating. Note that the size is
    // much larger than we actually obtain for each packet.
    //

    EncInstance.GetParam(EncHandle, VO_PID_ENC_VP8_OUTBUF_SIZE, &PictureSize);
    
    EncodedBuffer.Buffer = (VO_PBYTE)malloc(PictureSize);
    EncodedBuffer.Length = 0;

    //
    // Allocate raw input frame memory. In this demonstration example, we assume that 
    // default YUV format is YUV420.
    //

    PictureBuffer.ColorType = VO_COLOR_YUV_PLANAR420;
    AllocatePictureMemory(&PictureBuffer, Width, Height);
 
    //
    // Now read YUV stream into buffer and performs a packet encoding, until there is no more
    // available stream.
    //
   
    for( ;;) {

        FrameAvailable = ReadFrame(InFileStream, &PictureBuffer, Width, Height);

        //
        // Encode processing.
        //
        Result = EncInstance.Process( EncHandle, 
                                      FrameAvailable == VO_ERR_NONE ? &PictureBuffer : NULL, 
                                      &EncodedBuffer, 
                                      NULL );
        if (Result == VO_ERR_NONE) {

            assert(EncodedBuffer.Length > 0);

            AppendIvfPacketHeader(OutFileStream, &EncodedBuffer);

            fwrite(EncodedBuffer.Buffer, 1, EncodedBuffer.Length, OutFileStream);
        }
        else if (Result != VO_ERR_NONE && FrameAvailable != VO_ERR_NONE) {

            fprintf(stderr, "\nFrames Encoding complete!\n");

            break;
        }
    }

    fclose(InFileStream);
    fclose(OutFileStream);

    FreePictureMemory(&PictureBuffer);
    free(EncodedBuffer.Buffer);

    //
    // Un-initialize encoder handle, release internal resources.
    //

    EncInstance.Uninit(EncHandle);

    getchar();

    return VO_ERR_NONE;
}