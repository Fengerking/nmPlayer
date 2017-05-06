/*
 *  Copyright (c) 2010 The VP8 project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license and patent
 *  grant that can be found in the LICENSE file in the root of the source
 *  tree. All contributing project authors may be found in the AUTHORS
 *  file in the root of the source tree.
 */

#include "voVP8DecFindNearMV.h"


#define FINDNEAR_SEARCH_SITES   3

/* Predict motion vectors using those from already-decoded nearby blocks.
   Note that we only consider one 4x4 subblock from each candidate 16x16
   macroblock.   */

/*typedef union
{
    unsigned int as_int;
    MV           as_mv;
} int_mv; */       /* facilitates rapid equality tests */

//static void mv_bias(const MODE_INFO *x, int refframe, int_mv *mvp, const int *ref_frame_sign_bias)
//{
//    MV xmv;
//    xmv = x->mbmi.mv.as_mv;
//
//    if (ref_frame_sign_bias[x->mbmi.ref_frame] != ref_frame_sign_bias[refframe])
//    {
//        xmv.row *= -1;
//        xmv.col *= -1;
//    }
//
//    mvp->as_mv = xmv;
//}

//void vp8_find_near_mvs
//(
//    MACROBLOCKD *xd,
//    const MODE_INFO *here,
//    MV *nearest,
//    MV *nearby,
//    MV *best_mv,
//    //int cnt[4],
//	unsigned char *mv_ref_p,
//    int refframe,
//    int *ref_frame_sign_bias
//)
//{
//    const MODE_INFO *above = here - xd->mode_info_stride;
//    const MODE_INFO *left = here - 1;
//    const MODE_INFO *aboveleft = above - 1;
//    int_mv            near_mvs[4];
//    int_mv           *mv = near_mvs;
//	int cnt[4];
//    int             *cntx = cnt;
//    enum {CNT_INTRA, CNT_NEAREST, CNT_NEAR, CNT_SPLITMV};
//
//    /* Zero accumulators */
//    mv[0].as_int = mv[1].as_int = mv[2].as_int = 0;
//    cnt[0] = cnt[1] = cnt[2] = cnt[3] = 0;
//
//    /* Process above */  //near_mvs[1]
//    if (above->mbmi.ref_frame != INTRA_FRAME)
//    {
//        if (above->mbmi.mv.as_int)
//        {
//            (++mv)->as_int = above->mbmi.mv.as_int;
//
//			if (ref_frame_sign_bias[above->mbmi.ref_frame] != ref_frame_sign_bias[refframe])
//			{
//				mv->as_mv.row =  above->mbmi.mv.as_mv.row * -1;
//				mv->as_mv.col =  above->mbmi.mv.as_mv.col  * -1;
//			} 
//            ++cntx;
//        }
//        *cntx += 2;
//    }
//
//    /* Process left */
//    if (left->mbmi.ref_frame != INTRA_FRAME)
//    {
//        if (left->mbmi.mv.as_int)
//        {
//            int_mv this_mv;
//
//            this_mv.as_int = left->mbmi.mv.as_int;
//            //mv_bias(left, refframe, &this_mv, ref_frame_sign_bias);
//			if (ref_frame_sign_bias[left->mbmi.ref_frame] != ref_frame_sign_bias[refframe])
//			{
//				this_mv.as_mv.row =  left->mbmi.mv.as_mv.row * -1;
//				this_mv.as_mv.col =  left->mbmi.mv.as_mv.col  * -1;
//			}
//
//            if (this_mv.as_int != mv->as_int)
//            {
//                (++mv)->as_int = this_mv.as_int;
//                ++cntx;
//            }
//            *cntx += 2;
//        }
//        else
//            cnt[CNT_INTRA] += 2;
//    }
//
//    /* Process above left */
//    if (aboveleft->mbmi.ref_frame != INTRA_FRAME)
//    {
//        if (aboveleft->mbmi.mv.as_int)
//        {
//            int_mv this_mv;
//
//            this_mv.as_int = aboveleft->mbmi.mv.as_int;
//            //mv_bias(aboveleft, refframe, &this_mv, ref_frame_sign_bias);
//			if (ref_frame_sign_bias[aboveleft->mbmi.ref_frame] != ref_frame_sign_bias[refframe])
//			{
//				this_mv.as_mv.row =  aboveleft->mbmi.mv.as_mv.row * -1;
//				this_mv.as_mv.col =  aboveleft->mbmi.mv.as_mv.col  * -1;
//			}
//
//            if (this_mv.as_int != mv->as_int)
//            {
//                (++mv)->as_int = this_mv.as_int;
//                ++cntx;
//            }
//
//            *cntx += 1;
//        }
//        else
//            cnt[CNT_INTRA] += 1;
//    }
//
//    /* If we have three distinct MV's ... */
//    if (cnt[CNT_SPLITMV])
//    {
//        /* See if above-left MV can be merged with NEAREST */
//        if (mv->as_int == near_mvs[CNT_NEAREST].as_int)
//            cnt[CNT_NEAREST] += 1;
//    }
//
//    cnt[CNT_SPLITMV] = ((above->mbmi.mode == SPLITMV)
//                        + (left->mbmi.mode == SPLITMV)) * 2
//                       + (aboveleft->mbmi.mode == SPLITMV);
//
//    /* Swap near and nearest if necessary */
//    if (cnt[CNT_NEAR] > cnt[CNT_NEAREST])
//    {
//        int tmp;
//        tmp = cnt[CNT_NEAREST];
//        cnt[CNT_NEAREST] = cnt[CNT_NEAR];
//        cnt[CNT_NEAR] = tmp;
//
//        tmp = near_mvs[CNT_NEAREST].as_int;
//        near_mvs[CNT_NEAREST].as_int = near_mvs[CNT_NEAR].as_int;
//        near_mvs[CNT_NEAR].as_int = tmp;
//    }
//
//    /* Use near_mvs[0] to store the "best" MV */
//    if (cnt[CNT_NEAREST] >= cnt[CNT_INTRA])
//	{
//        near_mvs[CNT_INTRA] = near_mvs[CNT_NEAREST];
//	}
//
//	//if(above->mbmi.ref_frame == INTRA_FRAME && left->mbmi.ref_frame == INTRA_FRAME /*&& aboveleft->mbmi.ref_frame == INTRA_FRAME*/ )
//	//{
//	//	static int z=0;
//	//	printf("%d %d %d %d \n",cnt[0],cnt[1],cnt[2],cnt[3]);
//	//}
//
//    /* Set up return values */
//    *best_mv = near_mvs[0].as_mv;
//    *nearest = near_mvs[CNT_NEAREST].as_mv;
//    *nearby = near_mvs[CNT_NEAR].as_mv;
//
//    mv_ref_p[0] = vp8_mode_contexts [cnt[0]] [0];
//    mv_ref_p[1] = vp8_mode_contexts [cnt[1]] [1];
//    mv_ref_p[2] = vp8_mode_contexts [cnt[2]] [2];
//    mv_ref_p[3] = vp8_mode_contexts [cnt[3]] [3];
//
//    //vp8_clamp_mv(nearest, xd);
//    //vp8_clamp_mv(nearby, xd);
//    //vp8_clamp_mv(best_mv, xd); //TODO: move this up before the copy
//}


