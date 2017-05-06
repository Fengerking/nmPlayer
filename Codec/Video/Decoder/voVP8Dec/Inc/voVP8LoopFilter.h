/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __VP8_DEC_LOOPFILTER_H
#define __VP8_DEC_LOOPFILTER_H
#include "voType.h"
#include "voVP8GlobleDef.h"

#define NEW_FILTERMASK 1
typedef unsigned char uc;
#define MAX_LOOP_FILTER 63

#define ABS(a)  (((a)>0)?(a):(-(a)))

typedef enum
{
    NORMAL_LOOPFILTER = 0,
    SIMPLE_LOOPFILTER = 1
} LOOPFILTERTYPE;


// FRK
// Need to align this structure so when it is declared and
// passed it can be loaded into vector registers.
// FRK
typedef struct
{
    VO_U8  lim[16];
    VO_U8  flim[16];
    VO_U8  thr[16];
    VO_U8  mbflim[16];
} loop_filter_info;

#define prototype_loopfilter_block(sym) \
    void sym(VO_U8 *y, VO_U8 *u, VO_U8 *v,\
             int ystride, int uv_stride, loop_filter_info *lfi, int simpler)

static __inline signed char vp8_signed_char_clamp(int t)
{
    t = (t < -128 ? -128 : t);
    t = (t > 127 ? 127 : t);
    return (signed char) t;
}

static __inline VO_U8 vp8_unsigned_char_clamp(int t)
{
    t = (t < 0 ? 0 : t);
    t = (t > 255 ? 255 : t);
    return (VO_U8) t;
}


void vp8_loop_filter_mbhs(VO_U8 *y_ptr, VO_U8 *u_ptr, VO_U8 *v_ptr,
                            int y_stride, int uv_stride, loop_filter_info *lfi, int simpler_lpf);

void vp8_loop_filter_mbvs(VO_U8 *y_ptr, VO_U8 *u_ptr, VO_U8 *v_ptr,
                            int y_stride, int uv_stride, loop_filter_info *lfi, int simpler_lpf);

void vp8_loop_filter_bhs(VO_U8 *y_ptr, VO_U8 *u_ptr, VO_U8 *v_ptr,
                           int y_stride, int uv_stride, loop_filter_info *lfi, int simpler_lpf);

void vp8_loop_filter_bvs(VO_U8 *y_ptr, VO_U8 *u_ptr, VO_U8 *v_ptr,
                           int y_stride, int uv_stride, loop_filter_info *lfi, int simpler_lpf);

void vp8_mbloop_filter_horizontal_edge_c( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);
void vp8_loop_filter_horizontal_edge_c( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);
void vp8_mbloop_filter_vertical_edge_c( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);
void vp8_loop_filter_vertical_edge_c( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);


void vp8_mbloop_filter_horizontal_edge_armv6( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);
void vp8_mbloop_filter_horizontal_edge_armv6( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);
void vp8_mbloop_filter_horizontal_edge_armv6( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);
void vp8_mbloop_filter_vertical_edge_armv6( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);
void vp8_mbloop_filter_vertical_edge_armv6( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);
void vp8_mbloop_filter_vertical_edge_armv6( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);


void vp8_mbloop_filter_horizontal_edge_y_neon( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);
void vp8_mbloop_filter_horizontal_edge_uv_neon( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,VO_U8 *v_ptr);
void vp8_mbloop_filter_vertical_edge_y_neon( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);
void vp8_mbloop_filter_vertical_edge_uv_neon( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,VO_U8 *v_ptr);

void vp8_loop_filter_horizontal_edge_y_neon( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);
void vp8_loop_filter_horizontal_edge_uv_neon( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,VO_U8 *v_ptr);
void vp8_loop_filter_vertical_edge_y_neon( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,int count);
void vp8_loop_filter_vertical_edge_uv_neon( VO_U8 *s,int p,const VO_U8 *flimit,const VO_U8 *limit,const VO_U8 *thresh,VO_U8 *v_ptr);
void vp8_loop_filter_simple_vertical_edge_neon(VO_U8 *s,int p,const  VO_U8 *flimit,const  VO_U8 *limit,const  VO_U8 *thresh,int count);
void vp8_loop_filter_simple_horizontal_edge_neon(VO_U8 *s,int p,const  VO_U8 *flimit,const  VO_U8 *limit,const  VO_U8 *thresh,int count);
#endif