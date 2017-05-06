/*
 *  Copyright (c) 2010 The VP8 project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license and patent
 *  grant that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS
 *  file in the root of the source tree.
 */


#ifndef __VP8_FINDNEARMV_H
#define __VP8_FINDNEARMV_H
#include "voVP8GlobleDef.h"
#include "voVP8DecMBlock.h"


extern const signed char vp8_bmode_tree[18];
extern const signed char vp8_ymode_tree[8];
extern const signed char vp8_kf_ymode_tree[8];
extern const signed char vp8_uv_mode_tree[6];

#define vp8_read_bmode(bc,p)		vp8_treed_read(bc, vp8_bmode_tree, p)
#define vp8_read_ymode(bc,p)		vp8_treed_read(bc, vp8_ymode_tree, p)
#define vp8_kfread_ymode(bc,p)    vp8_treed_read(bc, vp8_kf_ymode_tree, p)
#define vp8_read_uv_mode(bc,p)    vp8_treed_read(bc, vp8_uv_mode_tree, p)

//void vp8_find_near_mvs
//(
//    MACROBLOCKD *xd,
//    const MODE_INFO *here,
//    MV *nearest, MV *nearby, MV *best,
//    //int near_mv_ref_cts[4],
//	unsigned char *mv_ref_p,
//    int refframe,
//    int *ref_frame_sign_bias
//);

//unsigned char *vp8_mv_ref_probs(
//    unsigned char p[VP8_MVREFS-1], const int near_mv_ref_ct[4]
//);

//const B_MODE_INFO *vp8_left_bmi(const MODE_INFO *cur_mb, int b);
//onst B_MODE_INFO *vp8_above_bmi(const MODE_INFO *cur_mb, int b, int mi_stride);
static INLINE const B_MODE_INFO *vp8_left_bmi( MODE_INFO *cur_mb, int b)
{
	MODE_INFO *left_mb = cur_mb-1;

	if(left_mb->mbmi.mode ==SPLITMV || left_mb->mbmi.mode<NEARESTMV )
	{
		if (!(b & 3))
		{
			b += 4;
			return left_mb->bmi + b - 1;
		}
		else
			return cur_mb->bmi + b - 1;
	}
	else
	{
		if (!(b & 3))
		{
			return left_mb->bmi;
		}
		else
			return cur_mb->bmi + b - 1;
	}
}

static INLINE const B_MODE_INFO *vp8_above_bmi(  MODE_INFO *cur_mb, int b, int mi_stride)
{
	MODE_INFO *above_mb = cur_mb-mi_stride;

	if(above_mb->mbmi.mode ==SPLITMV || above_mb->mbmi.mode== B_PRED )
	{
		if (!(b >> 2))
		{
			b += 16;
			return above_mb->bmi + b - 4;
		}
		else
			return cur_mb->bmi + b - 4;
	}
	else
	{
		if (!(b >> 2))
		{
			return above_mb->bmi;
		}
		else
			return cur_mb->bmi + b - 4;
	}
}


static INLINE const MV vp8_left_mv(  MODE_INFO *cur_mb, int b)
{
	MODE_INFO *left_mb = cur_mb-1;
	B_MODE_INFO * tmp;

	if(left_mb->mbmi.mode ==SPLITMV || left_mb->mbmi.mode<NEARESTMV )
	{
		if (!(b & 3))
		{
			b += 4;
			tmp =  left_mb->bmi + b - 1;
		}
		else
			tmp =  cur_mb->bmi + b - 1;
	}
	else
	{
		if (!(b & 3))
		{
			//tmp = left_mb->bmi;
			return left_mb->mbmi.mv.as_mv;
		}
		else
			tmp =  cur_mb->bmi + b - 1;
	}

	return tmp->mv.as_mv;
}

static INLINE const MV vp8_above_mv(  MODE_INFO *cur_mb, int b, int mi_stride)
{
	MODE_INFO *above_mb = cur_mb-mi_stride;
	B_MODE_INFO * tmp;

	if(above_mb->mbmi.mode ==SPLITMV || above_mb->mbmi.mode== B_PRED )
	{
		if (!(b >> 2))
		{
			b += 16;
			tmp = above_mb->bmi + b - 4;
		}
		else
			tmp = cur_mb->bmi + b - 4;
	}
	else
	{
		if (!(b >> 2))
		{
			//tmp = above_mb->mbmi;//above_mb->bmi;
			return above_mb->mbmi.mv.as_mv;
		}
		else
			tmp= cur_mb->bmi + b - 4;
	}
	return tmp->mv.as_mv;
}



#define LEFT_TOP_MARGIN (16 << 3)
#define RIGHT_BOTTOM_MARGIN (16 << 3)

#define vp8_clamp_mv(mv,mbmi) \
	if ((mv).col < ((mbmi)->mb_to_left_edge )) \
        (mv).col = (mbmi)->mb_to_left_edge ;\
    else if (mv.col > (mbmi)->mb_to_right_edge)\
        (mv).col = (mbmi)->mb_to_right_edge;\
    if ((mv).row < ((mbmi)->mb_to_top_edge))\
        (mv).row = (mbmi)->mb_to_top_edge ;\
    else if ((mv).row > (mbmi)->mb_to_bottom_edge )\
        (mv).row = (mbmi)->mb_to_bottom_edge ;


#define vp8_clamp_mv_uv(mvrow,mvcol,mbmi) \
	if (mvcol < ((mbmi).mb_to_left_edge>>1 )) \
        mvcol = ((mbmi).mb_to_left_edge>>1) ;\
    else if (mvcol > ((mbmi).mb_to_right_edge>>1))\
        mvcol = ((mbmi).mb_to_right_edge>>1);\
    if (mvrow < ((mbmi).mb_to_top_edge>>1))\
        mvrow = ((mbmi).mb_to_top_edge>>1) ;\
    else if (mvrow > ((mbmi).mb_to_bottom_edge>>1) )\
        mvrow = ((mbmi).mb_to_bottom_edge>>1) ;


#endif
