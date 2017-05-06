/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _IMAGEPROC_H_
#define _IMAGEPROC_H_

#include <stdlib.h>
#include "../voMpegEnc.h"
#define EDGE_SIZE  32

static VO_VOID __inline  FrameClean(Mpeg4Frame * frame)
{
	frame->y = frame->uv[0] = frame->uv[1] = NULL;
}

VO_S32 FrameCreate( ENCHND *enc_hnd, 
                            Mpeg4Frame * frame, 
                            VO_U32 edged_width, 
                            VO_U32 edged_height );

VO_VOID FrameDestroy( ENCHND *enc_hnd, 
                        Mpeg4Frame * frame, 
                        VO_U32 edged_width, 
                        VO_U32 edged_height );

VO_VOID FrameSetEdge( Mpeg4Frame * frame, 
                        const VO_U32 edged_width, 
                        const VO_U32 width, 
                        const VO_U32 height );

RETURN_CODE FramePreProcess( Mpeg4Frame * const img_cur,
						                const VO_U32 image_type,
						                const VO_IV_RTTYPE rt_type,
						                const VO_U32 width,
						                const VO_U32 height,
						                const VO_U32 dst_stride,
						                VO_U8 * src,
						                const VO_U32 src_stride );

#ifdef CAL_PSNR

VO_VOID FrameCopy( Mpeg4Frame * const img_dst,
		                    const Mpeg4Frame * const img_src,
		                    const VO_U32 stride,
		                    const VO_U32 height );

float FramePsnr(Mpeg4Frame * orig_image,
				    Mpeg4Frame * recon_image,
				    VO_U16 stride,
				    VO_U16 width,
				    VO_U16 height);

float SeeToPsnr(long sse, VO_S32 pixels);

long PlaneSSE(VO_U8 * orig,
		            VO_U8 * recon,
		            VO_U32 stride,
		            VO_U32 width,
		            VO_U32 height);
#endif

#endif							/* _IMAGE_H_ */
