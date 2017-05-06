/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#ifndef __VO_VP8DECFRONT_H__
#define __VO_VP8DECFRONT_H__

#include "stdlib.h"
#include "voVP8.h"

#define VPX_CODEC_DISABLE_COMPAT 1
#include "vpx_decoder.h"
#include "onyxd.h"
#include "onyxd_int.h"

typedef void   *VP8D_PTR;

typedef struct vo_vp8dec_ctx
{
	vpx_codec_ctx_t vpxcodec;

	VO_PTR*  pbi;
	VO_U8* framebuffer;
	VO_U32   framelength;
	VO_S32     nthreads;
	VO_S32 disable_deblock;
	VO_CODEC_INIT_USERDATA * pUserData;
	VO_S32 nCodecIdx;
	VO_S64     nTimes;
	VO_PTR  m_inUserData;
#ifdef LICENSEFILE
	VO_PTR* phCheck;//checkLib
	VO_VIDEO_BUFFER *pOutVideoBuf;
#endif
}VO_VP8DEC_CTX;

#endif
