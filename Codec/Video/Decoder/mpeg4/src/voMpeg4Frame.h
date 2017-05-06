/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef _VO_MPEG4_FRAME_H_
#define _VO_MPEG4_FRAME_H_
#include "voMpeg4DecGlobal.h"
#include "voMpeg4Dec.h"
#include "voMpegReadbits.h"
#include "voMpeg4MB.h"

/*H263*/
extern VO_U32 H263DecIVOP( VO_MPEG4_DEC* );
extern VO_U32 H263DecPVOP( VO_MPEG4_DEC* );
extern VO_U32 H263DecBVOP( VO_MPEG4_DEC* );

/*MPEG4*/
extern VO_U32 MPEG4DecIVOP( VO_MPEG4_DEC* );
extern VO_U32 MPEG4DecPVOP( VO_MPEG4_DEC* );
#ifdef ASP_BVOP
extern VO_U32 MPEG4DecBVOP( VO_MPEG4_DEC* );
extern VO_U32 MPEG4DecVOP( VO_MPEG4_DEC* );
extern VO_U32 MPEG4DecVOP_s( VO_MPEG4_DEC* );
#endif
#if ENABLE_DATA_PARTITION
extern VO_U32 MPEG4DecDPIVOP( VO_MPEG4_DEC* );
extern VO_U32 MPEG4DecDPPVOP( VO_MPEG4_DEC* );
extern VO_U32 MPEG4DecDPBVOP( VO_MPEG4_DEC* );
#endif//ENABLE_DATA_PARTITION

/*DIVX3*/
extern VO_U32 DIVX3DecIVOP( VO_MPEG4_DEC* );
extern VO_U32 DIVX3DecPVOP( VO_MPEG4_DEC* );
extern VO_U32 DIVX3DecBVOP( VO_MPEG4_DEC* );

#endif
