/****************************************************************************
 *        
 *   Module Title : yv12config.h
 *
 *   Description  : definition of a yv12 frame
 *
 *   Copyright (c) 1999 - 2005  On2 Technologies Inc. All Rights Reserved.
 *
 ***************************************************************************/
#ifndef YV12_CONFIG_H
#define YV12_CONFIG_H
#ifdef __cplusplus
extern "C"
{
#endif

#define VP7BORDERINPIXELS		48

typedef struct
{
    int   YWidth;
    int   YHeight;
    int   YStride;
//    int   YInternalWidth;

    int   UVWidth;
    int   UVHeight;
    int   UVStride;
//    int   UVInternalWidth;

    unsigned char *YBuffer;
	unsigned char *UBuffer;
    unsigned char *VBuffer;

    unsigned char *BufferAlloc;
    int border;
	int frameSize;

} YV12_BUFFER_CONFIG;

int On2YV12_AllocFrameBuffer(YV12_BUFFER_CONFIG *ybf,int width, int height,int border);
int On2YV12_DeAllocFrameBuffer(YV12_BUFFER_CONFIG *ybf);
int On2YV12_BlackFrameBuffer(YV12_BUFFER_CONFIG *ybf);

#ifdef __cplusplus
}
#endif


#endif //YV12_CONFIG_H

