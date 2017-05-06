/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2011	 					*
*																		*
************************************************************************/

#ifndef _VOH264FRAME_H_
#define _VOH264FRAME_H_
#include "voH264EncGlobal.h"

/* number of pixels past the edge of the frame, for motion estimation/compensation */
#define PADH 32
#define PADV 32

typedef struct AVC_FRAME
{
    VO_U8 *plane[3];
     
    /* for unrestricted mv we allocate more data than needed
     * allocated data are stored in buffer */
    VO_U8 *buffer[4];
} AVC_FRAME;

AVC_FRAME *AVCCrateFrame( H264ENC *pEncGlobal);
void  AVCDeleteFrame( AVC_FRAME *frame );

VO_S32 PreprocessPic( H264ENC *pEncGlobal, AVC_FRAME *dst, VO_VIDEO_BUFFER *src );

void FillBorder( H264ENC *pEncGlobal, AVC_FRAME *frame);


#endif

