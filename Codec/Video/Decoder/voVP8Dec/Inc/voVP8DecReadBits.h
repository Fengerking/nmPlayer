/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __VP8_DEC_READBITS_H
#define __VP8_DEC_READBITS_H
#include "voVP8GlobleDef.h"
#include "voVP8DecBoolCoder.h"

extern INLINE int vp8_treed_read_c(
    vp8_reader *const r,        /* !!! must return a 0 or 1 !!! */
    vp8_tree t,
    const unsigned char *const p
);
#define vp8_treed_read		vp8_treed_read_c

#endif