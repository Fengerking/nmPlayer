/****************************************************************************
 *        
 *   Module Title : yv12config.c
 *
 *   Description  : frame buffer allocation and deallocation
 *
 *   Copyright (c) 1999 - 2005  On2 Technologies Inc. All Rights Reserved.
 *
 ***************************************************************************/

/****************************************************************************
*  Header Files
****************************************************************************/
#include "yv12config.h"
#include "on2_mem.h"

/****************************************************************************
*  Exports
****************************************************************************/

/****************************************************************************
 *
 ****************************************************************************/
int 
On2YV12_DeAllocFrameBuffer(YV12_BUFFER_CONFIG *ybf)
{
    if(ybf)
    {
        if(ybf->BufferAlloc)
        {
            duck_free(ybf->BufferAlloc);
        }
        ybf->BufferAlloc =0;
    }
    else
    {
        return -1;
    }
    
    return 0;
}

/****************************************************************************
 *
 ****************************************************************************/
int 
On2YV12_AllocFrameBuffer(YV12_BUFFER_CONFIG *ybf, int width, int height, int border)
{
//NOTE: 

    int YPlaneSize = (height + 2 * border) * (width + 2 * border );
    int UVPlaneSize = ( height / 2 + border ) * (width / 2 + border);
    if(ybf)
    {
        On2YV12_DeAllocFrameBuffer(ybf);

        ybf->YWidth  = width;
        ybf->YHeight = height;
        ybf->YStride = width+2*border;

        ybf->UVWidth = width /2;
        ybf->UVHeight = height / 2;
        ybf->UVStride = ybf->UVWidth+border;

        ybf->border = border;
		ybf->frameSize = YPlaneSize * 3/2;

 	    // Added 2 extra lines to framebuffer so that copy12x12 doesn't fail
        // when we have a large motion vector in V on the last v block.  
        // Note : We never use these pixels anyway so this doesn't hurt.
        ybf->BufferAlloc = (unsigned char *) duck_memalign( 32,  (YPlaneSize * 3 / 2) +  (ybf->YStride * 2));
        if(ybf->BufferAlloc == NULL)    
            return -1;

        ybf->YBuffer = ybf->BufferAlloc + (border * ybf->YStride) + border;
        ybf->UBuffer = ybf->BufferAlloc + YPlaneSize + (border / 2  * ybf->UVStride) + border / 2;
        ybf->VBuffer = ybf->BufferAlloc + YPlaneSize + UVPlaneSize + (border / 2  * ybf->UVStride) + border / 2;
    }
    else
    {
        return -2;
    }
    return 0;
}

/****************************************************************************
 *
 ****************************************************************************/
int
On2YV12_BlackFrameBuffer(YV12_BUFFER_CONFIG *ybf)
{
    if(ybf)
    {
        if(ybf->BufferAlloc)
        {
            duck_memset(ybf->YBuffer, 0x0, ybf->YStride * ybf->YHeight);                 
            duck_memset(ybf->UBuffer, 0x80, ybf->UVStride * ybf->UVHeight);                                   
            duck_memset(ybf->VBuffer, 0x80, ybf->UVStride * ybf->UVHeight);    
        }

        return 0;
    }
    return -1;
}

