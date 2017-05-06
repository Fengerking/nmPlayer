/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "voMpegPort.h"
#include "voMpeg2Decoder.h"

//#define SAFETY	32
#define EDGE_SIZE  0
#define EDGE_SIZE2  (EDGE_SIZE/2)

static VO_VOID __inline
image_null(Image * image)
{
	image->y = image->u = image->v = NULL;
}

VO_S32 image_create(Image * image,
					 VO_U32 edged_width,
					 VO_U32 edged_height,
					 MpegDecode* dec);
VO_VOID image_destroy(Image * image,
				   VO_U32 edged_width,
				   VO_U32 edged_height,
				   MpegDecode* dec);

VO_VOID image_swap(Image * image1,	Image * image2);

#endif							/* _IMAGE_H_ */
