/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */


#include "vpx_config.h"
#include "vpx_rtcd.h"
#include "onyxd_int.h"
#include "vp8/common/header.h"
#include "vp8/common/reconintra4x4.h"
#include "vp8/common/reconinter.h"
#include "detokenize.h"
#include "vp8/common/invtrans.h"
#include "vp8/common/alloccommon.h"
#include "vp8/common/entropymode.h"
#include "vp8/common/quant_common.h"
#include "vpx_scale/vpxscale.h"
#include "vp8/common/setupintrarecon.h"

#include "decodemv.h"
#include "vp8/common/extend.h"
#if CONFIG_ERROR_CONCEALMENT
#include "error_concealment.h"
#endif
#include "vpx_mem/vpx_mem.h"
#include "vp8/common/threading.h"
#include "decoderthreading.h"
#include "dboolhuff.h"

#include <assert.h>
#include <stdio.h>

extern int swap_frame_buffers (VP8_COMMON *cm,int mul_thd);
extern int vp8_alloc_priv_buffer(VP8_COMMON *oci, int width, int height);

void vp8cx_init_de_quantizer(VP8D_COMP *pbi)
{
    int Q;
    VP8_COMMON *const pc = & pbi->common;

    for (Q = 0; Q < QINDEX_RANGE; Q++)
    {
        pc->Y1dequant[Q][0] = (short)vp8_dc_quant(Q, pc->y1dc_delta_q);
        pc->Y2dequant[Q][0] = (short)vp8_dc2quant(Q, pc->y2dc_delta_q);
        pc->UVdequant[Q][0] = (short)vp8_dc_uv_quant(Q, pc->uvdc_delta_q);

        pc->Y1dequant[Q][1] = (short)vp8_ac_yquant(Q);
        pc->Y2dequant[Q][1] = (short)vp8_ac2quant(Q, pc->y2ac_delta_q);
        pc->UVdequant[Q][1] = (short)vp8_ac_uv_quant(Q, pc->uvac_delta_q);
    }
}

void vp8_mb_init_dequantizer(VP8D_COMP *pbi, MACROBLOCKD *xd)
{
    int i;
    int QIndex;
    MB_MODE_INFO *mbmi = &xd->mode_info_context->mbmi;
    VP8_COMMON *const pc = & pbi->common;

    /* Decide whether to use the default or alternate baseline Q value. */
    if (xd->segmentation_enabled)
    {
        /* Abs Value */
        if (xd->mb_segement_abs_delta == SEGMENT_ABSDATA)
            QIndex = xd->segment_feature_data[MB_LVL_ALT_Q][mbmi->segment_id];

        /* Delta Value */
        else
        {
            QIndex = pc->base_qindex + xd->segment_feature_data[MB_LVL_ALT_Q][mbmi->segment_id];
            QIndex = (QIndex >= 0) ? ((QIndex <= MAXQ) ? QIndex : MAXQ) : 0;    /* Clamp to valid range */
        }
    }
    else
        QIndex = pc->base_qindex;

    /* Set up the macroblock dequant constants */
    xd->dequant_y1_dc[0] = 1;
    xd->dequant_y1[0] = pc->Y1dequant[QIndex][0];
    xd->dequant_y2[0] = pc->Y2dequant[QIndex][0];
    xd->dequant_uv[0] = pc->UVdequant[QIndex][0];

    for (i = 1; i < 16; i++)
    {
        xd->dequant_y1_dc[i] =
        xd->dequant_y1[i] = pc->Y1dequant[QIndex][1];
        xd->dequant_y2[i] = pc->Y2dequant[QIndex][1];
        xd->dequant_uv[i] = pc->UVdequant[QIndex][1];
    }
}

static void decode_macroblock(VP8D_COMP *pbi, MACROBLOCKD *xd,
                              unsigned int mb_idx)
{
    MB_PREDICTION_MODE mode;
    int i,eobtotal=0;
#if CONFIG_ERROR_CONCEALMENT
    int corruption_detected = 0;
#endif
    if (xd->mode_info_context->mbmi.mb_skip_coeff)
    {
        vp8_reset_mb_tokens_context(xd);
		xd->mode_info_context->mbmi.mb_skip_coeff_y = 1;
		xd->mode_info_context->mbmi.mb_skip_coeff_uv =1;
    }
    else if (!vp8dx_bool_error(xd->current_bc))
    {
        eobtotal = vp8_decode_mb_tokens(pbi, xd);

        /* Special case:  Force the loopfilter to skip when eobtotal is zero */
        xd->mode_info_context->mbmi.mb_skip_coeff = (eobtotal==0);
    }

    mode = xd->mode_info_context->mbmi.mode;

#ifdef VOOPT
    if (xd->segmentation_enabled && !xd->mode_info_context->mbmi.mb_skip_coeff)
#else
	if (xd->segmentation_enabled)
#endif
        vp8_mb_init_dequantizer(pbi, xd);

    /* do prediction */
    if (xd->mode_info_context->mbmi.ref_frame == INTRA_FRAME)
    {
        vp8_build_intra_predictors_mbuv_s(xd,
                                          xd->recon_above[1],
                                          xd->recon_above[2],
                                          xd->recon_left[1],
                                          xd->recon_left[2],
                                          xd->recon_left_stride[1],
                                          xd->dst.u_buffer, xd->dst.v_buffer,
                                          xd->dst.uv_stride);

        if (mode != B_PRED)
        {
            vp8_build_intra_predictors_mby_s(xd,
                                                 xd->recon_above[0],
                                                 xd->recon_left[0],
                                                 xd->recon_left_stride[0],
                                                 xd->dst.y_buffer,
                                                 xd->dst.y_stride);
        }
        else
        {
            short *DQC = xd->dequant_y1;
            int dst_stride = xd->dst.y_stride;
            unsigned char *base_dst = xd->dst.y_buffer;

            /* clear out residual eob info */
            if(xd->mode_info_context->mbmi.mb_skip_coeff_y)
                vpx_memset(xd->eobs, 0, 16);

            intra_prediction_down_copy(xd, xd->recon_above[0] + 16);

            for (i = 0; i < 16; i++)
            {
                BLOCKD *b = &xd->block[i];
                int b_mode = xd->mode_info_context->bmi[i].as_mode;
                unsigned char *yabove;
                unsigned char *yleft;
                int left_stride;
                unsigned char top_left;
#if USE_FRAME_THREAD
				if (pbi->nThdNum > 1)
				{
					/*Caution: For some b_mode, it needs 8 pixels (4 above + 4 above-right).*/
					if (i < 4 && pbi->common.filter_level)
						yabove = xd->recon_above[0] + b->offset; //i*4;
					else
						yabove = (base_dst - dst_stride) + b->offset;

					if (i%4==0 && pbi->common.filter_level)
					{
						yleft = xd->recon_left[0] + i;
						left_stride = 1;
					}
					else
					{
						yleft = (base_dst  - 1) + b->offset;
						left_stride = dst_stride;
					}

					if ((i==4 || i==8 || i==12) && pbi->common.filter_level)
						top_left = *(xd->recon_left[0] + i - 1);
					else
						top_left = yabove[-1];
				}
				else
#endif
				{
					yabove = base_dst + b->offset - dst_stride;
					yleft = base_dst + b->offset - 1;
					left_stride = dst_stride;
					top_left = yabove[-1];
				}

                // vp8_intra4x4_predict (base_dst + b->offset, dst_stride, b_mode,
                //                                    base_dst + b->offset, dst_stride );
                vp8_intra4x4_predict_d_c(yabove, yleft, left_stride,
                                       b_mode,
                                       base_dst + b->offset, dst_stride,
                                       top_left);

                if (xd->eobs[i])
                {
                    if (xd->eobs[i] > 1)
                    {
                    vp8_dequant_idct_add
                            (b->qcoeff, DQC,
                                base_dst + b->offset, dst_stride);
                    }
                    else
                    {
                        vp8_dc_only_idct_add
                            (b->qcoeff[0] * DQC[0],
                                base_dst + b->offset, dst_stride,
                                base_dst + b->offset, dst_stride);
                        ((int *)b->qcoeff)[0] = 0;
                    }
                }
            }
        }
    }
    else
    {
        vp8_build_inter_predictors_mb(pbi, xd);
    }


#if CONFIG_ERROR_CONCEALMENT
    if (corruption_detected)
    {
        return;
    }
#endif

#ifdef VOOPT
	if(!xd->mode_info_context->mbmi.mb_skip_coeff_y)
	{
		if (mode != B_PRED)
        {
            short *DQC = xd->dequant_y1;

            if (mode != SPLITMV)
            {
                BLOCKD *b = &xd->block[24];

                /* do 2nd order transform on the dc block */
				if(xd->eobs[24] == 1)
				{
					b->dqcoeff[0] = b->qcoeff[0] * xd->dequant_y2[0];
					vp8_short_inv_walsh4x4_1(&b->dqcoeff[0],
						xd->qcoeff);
					((int *)b->qcoeff)[0] = 0;
				}
				else
				{
					 if (xd->eobs[24] > 1)
					{
						vp8_dequantize_b(b, xd->dequant_y2);

						vp8_short_inv_walsh4x4(&b->dqcoeff[0],
							xd->qcoeff);
						((int *)b->qcoeff)[0] = 0;
						((int *)b->qcoeff)[1] = 0;
						((int *)b->qcoeff)[2] = 0;
						((int *)b->qcoeff)[3] = 0;
						((int *)b->qcoeff)[4] = 0;
						((int *)b->qcoeff)[5] = 0;
						((int *)b->qcoeff)[6] = 0;
						((int *)b->qcoeff)[7] = 0;
					}
				}
                /* override the dc dequant constant in order to preserve the
                 * dc components
                 */
                DQC = xd->dequant_y1_dc;
            }

            vp8_dequant_idct_add_y_block
                            (xd->qcoeff, DQC,
                             xd->dst.y_buffer,
                             xd->dst.y_stride, xd->eobs);
        }
	}

	if(!xd->mode_info_context->mbmi.mb_skip_coeff_uv)
	{
		vp8_dequant_idct_add_uv_block
                        (xd->qcoeff+16*16, xd->dequant_uv,
                         xd->dst.u_buffer, xd->dst.v_buffer,
                         xd->dst.uv_stride, xd->eobs+16);
	}
#else
    if(!xd->mode_info_context->mbmi.mb_skip_coeff)
    {
        /* dequantization and idct */
        if (mode != B_PRED)
        {
            short *DQC = xd->dequant_y1;

            if (mode != SPLITMV)
            {
                BLOCKD *b = &xd->block[24];

                /* do 2nd order transform on the dc block */
                if (xd->eobs[24] > 1)
                {
                    vp8_dequantize_b(b, xd->dequant_y2);

                    vp8_short_inv_walsh4x4(&b->dqcoeff[0],
                        xd->qcoeff);
                    ((int *)b->qcoeff)[0] = 0;
                    ((int *)b->qcoeff)[1] = 0;
                    ((int *)b->qcoeff)[2] = 0;
                    ((int *)b->qcoeff)[3] = 0;
                    ((int *)b->qcoeff)[4] = 0;
                    ((int *)b->qcoeff)[5] = 0;
                    ((int *)b->qcoeff)[6] = 0;
                    ((int *)b->qcoeff)[7] = 0;
                }
                else
                {
					if(b->qcoeff[0] == 0)
						b->qcoeff[0] =b->qcoeff[0];
                    b->dqcoeff[0] = b->qcoeff[0] * xd->dequant_y2[0];
                    vp8_short_inv_walsh4x4_1(&b->dqcoeff[0],
                        xd->qcoeff);
                    ((int *)b->qcoeff)[0] = 0;
                }

                /* override the dc dequant constant in order to preserve the
                 * dc components
                 */
                DQC = xd->dequant_y1_dc;
            }

            vp8_dequant_idct_add_y_block
                            (xd->qcoeff, DQC,
                             xd->dst.y_buffer,
                             xd->dst.y_stride, xd->eobs);
        }

        vp8_dequant_idct_add_uv_block
                        (xd->qcoeff+16*16, xd->dequant_uv,
                         xd->dst.u_buffer, xd->dst.v_buffer,
                         xd->dst.uv_stride, xd->eobs+16);
    }
#endif
}

int get_delta_q(vp8_reader *bc, int prev, int *q_update)
{
    int ret_val = 0;

    if (vp8_read_bit(bc))
    {
        ret_val = vp8_read_literal(bc, 4);

        if (vp8_read_bit(bc))
            ret_val = -ret_val;
    }

    /* Trigger a quantizer update if the delta-q value has changed */
    if (ret_val != prev)
        *q_update = 1;

    return ret_val;
}

#ifdef PACKET_TESTING
#include <stdio.h>
FILE *vpxlog = 0;
#endif

static void decode_mb_rows(VP8D_COMP *pbi)
{
    VP8_COMMON *const pc = & pbi->common;
    MACROBLOCKD *const xd  = & pbi->mb;

    int ibc = 0;
    int num_part = 1 << pc->multi_token_partition;

    int recon_yoffset, recon_uvoffset;
    int mb_row, mb_col;
    int mb_idx = 0;
    int dst_fb_idx = pc->new_fb_idx;
    int recon_y_stride = pc->yv12_fb[dst_fb_idx].y_stride;
    int recon_uv_stride = pc->yv12_fb[dst_fb_idx].uv_stride;

    unsigned char *ref_buffer[MAX_REF_FRAMES][3];
    unsigned char *dst_buffer[3];
    int i;
    int ref_fb_index[MAX_REF_FRAMES];
    int ref_fb_corrupted[MAX_REF_FRAMES];

    ref_fb_corrupted[INTRA_FRAME] = 0;

#if USE_FRAME_THREAD
	if (pbi->nThdNum > 1)
	{
		ref_fb_index[LAST_FRAME]    = pc->lst_fb_idx_s;
		ref_fb_index[GOLDEN_FRAME]  = pc->gld_fb_idx_s;
		ref_fb_index[ALTREF_FRAME]  = pc->alt_fb_idx_s;
	}
	else
#endif
	{
		ref_fb_index[LAST_FRAME]    = pc->lst_fb_idx;
		ref_fb_index[GOLDEN_FRAME]  = pc->gld_fb_idx;
		ref_fb_index[ALTREF_FRAME]  = pc->alt_fb_idx;
	}

    for(i = 1; i < MAX_REF_FRAMES; i++)
    {
        ref_buffer[i][0] = pc->yv12_fb[ref_fb_index[i]].y_buffer;
        ref_buffer[i][1] = pc->yv12_fb[ref_fb_index[i]].u_buffer;
        ref_buffer[i][2] = pc->yv12_fb[ref_fb_index[i]].v_buffer;

        ref_fb_corrupted[i] = pc->yv12_fb[ref_fb_index[i]].corrupted;
    }

    dst_buffer[0] = pc->yv12_fb[dst_fb_idx].y_buffer;
    dst_buffer[1] = pc->yv12_fb[dst_fb_idx].u_buffer;
    dst_buffer[2] = pc->yv12_fb[dst_fb_idx].v_buffer;

    xd->up_available = 0;

#if USE_FRAME_THREAD
// 	pc->filter_level = 0;	//to disable loopfilter
	if (pbi->nThdNum > 1 && pc->filter_level)
	{
		/* Set above_row buffer to 127 for decoding first MB row */
		vpx_memset(pbi->mt_yabove_row[0] + VP8BORDERINPIXELS-1, 127, pc->yv12_fb[pc->lst_fb_idx_s].y_width + 5);
		vpx_memset(pbi->mt_uabove_row[0] + (VP8BORDERINPIXELS>>1)-1, 127, (pc->yv12_fb[pc->lst_fb_idx_s].y_width>>1) +5);
		vpx_memset(pbi->mt_vabove_row[0] + (VP8BORDERINPIXELS>>1)-1, 127, (pc->yv12_fb[pc->lst_fb_idx_s].y_width>>1) +5);

		for (i=1; i<pc->mb_rows; i++)
		{
			vpx_memset(pbi->mt_yabove_row[i] + VP8BORDERINPIXELS-1, (unsigned char)129, 1);
			vpx_memset(pbi->mt_uabove_row[i] + (VP8BORDERINPIXELS>>1)-1, (unsigned char)129, 1);
			vpx_memset(pbi->mt_vabove_row[i] + (VP8BORDERINPIXELS>>1)-1, (unsigned char)129, 1);
		}

		/* Set left_col to 129 initially */
		for (i=0; i<pc->mb_rows; i++)
		{
			vpx_memset(pbi->mt_yleft_col[i], (unsigned char)129, 16);
			vpx_memset(pbi->mt_uleft_col[i], (unsigned char)129, 8);
			vpx_memset(pbi->mt_vleft_col[i], (unsigned char)129, 8);
		}

		/* Initialize the loop filter for this frame. */
		vp8_loop_filter_frame_init(pc, &pbi->mb, pc->filter_level);
	}
#endif

    /* Decode the individual macro block */
    for (mb_row = 0; mb_row < pc->mb_rows; mb_row++)
    {
        if (num_part > 1)
        {
            xd->current_bc = & pbi->mbc[ibc];
            ibc++;

            if (ibc == num_part)
                ibc = 0;
        }

        recon_yoffset = mb_row * recon_y_stride * 16;
        recon_uvoffset = mb_row * recon_uv_stride * 8;

        /* reset contexts */
        xd->above_context = pc->above_context;
        vpx_memset(xd->left_context, 0, sizeof(ENTROPY_CONTEXT_PLANES));

        xd->left_available = 0;

        xd->mb_to_top_edge = -((mb_row * 16)) << 3;
        xd->mb_to_bottom_edge = ((pc->mb_rows - 1 - mb_row) * 16) << 3;

#if USE_FRAME_THREAD
		if (pbi->nThdNum > 1 && pbi->common.filter_level)
		{
			xd->recon_above[0] = pbi->mt_yabove_row[mb_row] + 0*16 +32;
			xd->recon_above[1] = pbi->mt_uabove_row[mb_row] + 0*8 +16;
			xd->recon_above[2] = pbi->mt_vabove_row[mb_row] + 0*8 +16;

			xd->recon_left[0] = pbi->mt_yleft_col[mb_row];
			xd->recon_left[1] = pbi->mt_uleft_col[mb_row];
			xd->recon_left[2] = pbi->mt_vleft_col[mb_row];

			//TODO: move to outside row loop
			xd->recon_left_stride[0] = 1;
			xd->recon_left_stride[1] = 1;
		}
		else
#endif
		{
			xd->recon_above[0] = dst_buffer[0] + recon_yoffset;
			xd->recon_above[1] = dst_buffer[1] + recon_uvoffset;
			xd->recon_above[2] = dst_buffer[2] + recon_uvoffset;

			xd->recon_left[0] = xd->recon_above[0] - 1;
			xd->recon_left[1] = xd->recon_above[1] - 1;
			xd->recon_left[2] = xd->recon_above[2] - 1;

			xd->recon_above[0] -= xd->dst.y_stride;
			xd->recon_above[1] -= xd->dst.uv_stride;
			xd->recon_above[2] -= xd->dst.uv_stride;

			//TODO: move to outside row loop
			xd->recon_left_stride[0] = xd->dst.y_stride;
			xd->recon_left_stride[1] = xd->dst.uv_stride;
		}

        for (mb_col = 0; mb_col < pc->mb_cols; mb_col++)
        {
            /* Distance of Mb to the various image edges.
             * These are specified to 8th pel as they are always compared to values
             * that are in 1/8th pel units
             */
            xd->mb_to_left_edge = -((mb_col * 16) << 3);
            xd->mb_to_right_edge = ((pc->mb_cols - 1 - mb_col) * 16) << 3;

#if CONFIG_ERROR_CONCEALMENT
            {
                int corrupt_residual = (!pbi->independent_partitions/* && pbi->frame_corrupt_residual*/) ||
                                       vp8dx_bool_error(xd->current_bc);
                if (pbi->ec_active &&
                    xd->mode_info_context->mbmi.ref_frame == INTRA_FRAME &&
                    corrupt_residual)
                {
                    /* We have an intra block with corrupt coefficients, better to
                     * conceal with an inter block. Interpolate MVs from neighboring
                     * MBs.
                     *
                     * Note that for the first mb with corrupt residual in a frame,
                     * we might not discover that before decoding the residual. That
                     * happens after this check, and therefore no inter concealment
                     * will be done.
                     */
                    vp8_interpolate_motion(xd,
                                           mb_row, mb_col,
                                           pc->mb_rows, pc->mb_cols,
                                           pc->mode_info_stride);
                }
            }
#endif

            xd->dst.y_buffer = dst_buffer[0] + recon_yoffset;
            xd->dst.u_buffer = dst_buffer[1] + recon_uvoffset;
            xd->dst.v_buffer = dst_buffer[2] + recon_uvoffset;

            xd->pre.y_buffer = ref_buffer[xd->mode_info_context->mbmi.ref_frame][0] + recon_yoffset;
            xd->pre.u_buffer = ref_buffer[xd->mode_info_context->mbmi.ref_frame][1] + recon_uvoffset;
            xd->pre.v_buffer = ref_buffer[xd->mode_info_context->mbmi.ref_frame][2] + recon_uvoffset;

			xd->pRef = pc->yv12_fb + ref_fb_index[xd->mode_info_context->mbmi.ref_frame];

            /* propagate errors from reference frames */
            xd->corrupted |= ref_fb_corrupted[xd->mode_info_context->mbmi.ref_frame];

            decode_macroblock(pbi, xd, mb_idx);

            xd->left_available = 1;

            /* check if the boolean decoder has suffered an error */
            xd->corrupted |= vp8dx_bool_error(xd->current_bc);

            xd->recon_above[0] += 16;
            xd->recon_above[1] += 8;
            xd->recon_above[2] += 8;

#if USE_FRAME_THREAD
			if (pbi->nThdNum > 1 && pbi->common.filter_level)
			{
				int skip_lf = (xd->mode_info_context->mbmi.mode != B_PRED &&
					xd->mode_info_context->mbmi.mode != SPLITMV &&
					xd->mode_info_context->mbmi.mb_skip_coeff);

				loop_filter_info_n *lfi_n = &pc->lf_info;
				const int mode_index = lfi_n->mode_lf_lut[xd->mode_info_context->mbmi.mode];
				const int seg = xd->mode_info_context->mbmi.segment_id;
				const int ref_frame = xd->mode_info_context->mbmi.ref_frame;

				int filter_level = lfi_n->lvl[seg][ref_frame][mode_index];

				if( mb_row != pc->mb_rows-1 )
				{
					/* Save decoded MB last row data for next-row decoding */
					vpx_memcpy((pbi->mt_yabove_row[mb_row + 1] + 32 + mb_col*16), (xd->dst.y_buffer + 15 * recon_y_stride), 16);
					vpx_memcpy((pbi->mt_uabove_row[mb_row + 1] + 16 + mb_col*8), (xd->dst.u_buffer + 7 * recon_uv_stride), 8);
					vpx_memcpy((pbi->mt_vabove_row[mb_row + 1] + 16 + mb_col*8), (xd->dst.v_buffer + 7 * recon_uv_stride), 8);
				}

				/* save left_col for next MB decoding */
				if(mb_col != pc->mb_cols-1)
				{
					MODE_INFO *next = xd->mode_info_context +1;

					if (next->mbmi.ref_frame == INTRA_FRAME)
					{
						for (i = 0; i < 16; i++)
							pbi->mt_yleft_col[mb_row][i] = xd->dst.y_buffer [i* recon_y_stride + 15];
						for (i = 0; i < 8; i++)
						{
							pbi->mt_uleft_col[mb_row][i] = xd->dst.u_buffer [i* recon_uv_stride + 7];
							pbi->mt_vleft_col[mb_row][i] = xd->dst.v_buffer [i* recon_uv_stride + 7];
						}
					}
				}

				/* loopfilter on this macroblock. */
				if (filter_level)
				{
					if(pc->filter_type == NORMAL_LOOPFILTER)
					{
						loop_filter_info lfi;
						FRAME_TYPE frame_type = pc->frame_type;
						const int hev_index = lfi_n->hev_thr_lut[frame_type][filter_level];
						lfi.mblim = lfi_n->mblim[filter_level];
						lfi.blim = lfi_n->blim[filter_level];
						lfi.lim = lfi_n->lim[filter_level];
						lfi.hev_thr = lfi_n->hev_thr[hev_index];

						if (mb_col > 0)
							vp8_loop_filter_mbv
							(xd->dst.y_buffer, xd->dst.u_buffer, xd->dst.v_buffer, recon_y_stride, recon_uv_stride, &lfi);

						if (!skip_lf)
							vp8_loop_filter_bv
							(xd->dst.y_buffer, xd->dst.u_buffer, xd->dst.v_buffer, recon_y_stride, recon_uv_stride, &lfi);

						/* don't apply across umv border */
						if (mb_row > 0)
							vp8_loop_filter_mbh
							(xd->dst.y_buffer, xd->dst.u_buffer, xd->dst.v_buffer, recon_y_stride, recon_uv_stride, &lfi);

						if (!skip_lf)
							vp8_loop_filter_bh
							(xd->dst.y_buffer, xd->dst.u_buffer, xd->dst.v_buffer,  recon_y_stride, recon_uv_stride, &lfi);
					}
					else
					{
						if (mb_col > 0)
							vp8_loop_filter_simple_mbv
							(xd->dst.y_buffer, recon_y_stride, lfi_n->mblim[filter_level]);

						if (!skip_lf)
							vp8_loop_filter_simple_bv
							(xd->dst.y_buffer, recon_y_stride, lfi_n->blim[filter_level]);

						/* don't apply across umv border */
						if (mb_row > 0)
							vp8_loop_filter_simple_mbh
							(xd->dst.y_buffer, recon_y_stride, lfi_n->mblim[filter_level]);

						if (!skip_lf)
							vp8_loop_filter_simple_bh
							(xd->dst.y_buffer, recon_y_stride, lfi_n->blim[filter_level]);
					}
				}

			}
			else
#endif
			{
				xd->recon_left[0] += 16;
				xd->recon_left[1] += 8;
				xd->recon_left[2] += 8;
			}


            recon_yoffset += 16;
            recon_uvoffset += 8;

			mb_idx++;
            ++xd->mode_info_context;  /* next mb */

            xd->above_context++;

        }

        /* adjust to the next row of mbs */
#if USE_FRAME_THREAD
		if (pbi->nThdNum > 1 && pbi->common.filter_level)
		{
			if(mb_row != pc->mb_rows-1)
			{
				int lasty = pc->yv12_fb[pc->lst_fb_idx_s].y_width + VP8BORDERINPIXELS;
				int lastuv = (pc->yv12_fb[pc->lst_fb_idx_s].y_width>>1) + (VP8BORDERINPIXELS>>1);

				for (i = 0; i < 4; i++)
				{
					pbi->mt_yabove_row[mb_row +1][lasty + i] = pbi->mt_yabove_row[mb_row +1][lasty -1];
					pbi->mt_uabove_row[mb_row +1][lastuv + i] = pbi->mt_uabove_row[mb_row +1][lastuv -1];
					pbi->mt_vabove_row[mb_row +1][lastuv + i] = pbi->mt_vabove_row[mb_row +1][lastuv -1];
				}
			}
		} else
#endif
		{
			vp8_extend_mb_row(
				&pc->yv12_fb[dst_fb_idx],
				xd->dst.y_buffer + 16, xd->dst.u_buffer + 8, xd->dst.v_buffer + 8
				);
		}

        ++xd->mode_info_context;      /* skip prediction column */
        xd->up_available = 1;

#if USE_FRAME_THREAD
		if (pbi->nThdNum > 1)
		{
			//frame extend by mb row
			unsigned char *y_buffer = xd->dst.y_buffer - 16*(pc->mb_cols-1);
			unsigned char *u_buffer = xd->dst.u_buffer - 8*(pc->mb_cols-1);
			unsigned char *v_buffer = xd->dst.v_buffer - 8*(pc->mb_cols-1);
			unsigned char *y_end;
			unsigned char *u_end;
			unsigned char *v_end;
			int lf_rows;
			unsigned int Border = xd->dst.border;
			int nProgress;

			if (pbi->common.filter_level)
			{
				if(pc->filter_type == NORMAL_LOOPFILTER)
				{
					lf_rows = 8;
				}
				else
				{
					lf_rows = 4;
				}
			}
			else
			{
				lf_rows = 2;
			}
// 			lf_rows = 16;

			if (mb_row != pc->mb_rows-1)
			{
				y_end = y_buffer + (16-lf_rows)*recon_y_stride;
				u_end = u_buffer + (8-lf_rows/2)*recon_uv_stride;
				v_end = v_buffer + (8-lf_rows/2)*recon_uv_stride;
			}
			else
			{
				y_end = y_buffer + 16*recon_y_stride;
				u_end = u_buffer + 8*recon_uv_stride;
				v_end = v_buffer + 8*recon_uv_stride;
// 				y_end = dst_buffer[0] + xd->dst.y_height*recon_y_stride;
// 				u_end = dst_buffer[1] + xd->dst.uv_height*recon_uv_stride;
// 				v_end = dst_buffer[2] + xd->dst.uv_height*recon_uv_stride;
			}

			if (mb_row > 0)
			{
				y_buffer -= lf_rows*recon_y_stride;
				u_buffer -= lf_rows/2*recon_uv_stride;
				v_buffer -= lf_rows/2*recon_uv_stride;
			}
			
			//Y plane extend
			//extend left & right
			for (;y_buffer != y_end; y_buffer += recon_y_stride)
			{
				unsigned char *s = y_buffer;
				unsigned char *d = s - Border;
				memset(d, *s, Border);
				d = s + xd->dst.y_width;
				s = d-1;
				memset(d, *s, Border);
			}
			if (mb_row == 0)
			{
				//extend up
				y_end = dst_buffer[0] - Border;
				y_buffer = y_end - Border*recon_y_stride;
				for (;y_buffer != y_end; y_buffer += recon_y_stride)
				{
					memcpy(y_buffer, y_end, recon_y_stride);
				}
			}
			else if (mb_row == pc->mb_rows-1)
			{
				//extend down
				const unsigned char * const s = y_end - Border - recon_y_stride;
				y_buffer -= Border;
				y_end = y_buffer + Border*recon_y_stride;
				for (;y_buffer != y_end; y_buffer += recon_y_stride)
				{
					memcpy(y_buffer, s, recon_y_stride);
				}
			}

			Border /= 2;
			//U plane extend
			//left & right
			for (;u_buffer != u_end; u_buffer += recon_uv_stride)
			{
				unsigned char *s = u_buffer;
				unsigned char *d = s - Border;
				memset(d, *s, Border);
				d = s + xd->dst.uv_width;
				s = d-1;
				memset(d, *s, Border);
			}
			if (mb_row == 0)
			{
				//extend up
				u_end = dst_buffer[1] - Border;
				u_buffer = u_end - Border*recon_uv_stride;
				for (;u_buffer != u_end; u_buffer += recon_uv_stride)
				{
					memcpy(u_buffer, u_end, recon_uv_stride);
				}
			}
			else if (mb_row == pc->mb_rows-1)
			{
				//extend down
				const unsigned char * const s = u_end - Border - recon_uv_stride;
				u_buffer -= Border;
				u_end = u_buffer + Border*recon_uv_stride;
				for (;u_buffer != u_end; u_buffer += recon_uv_stride)
				{
					memcpy(u_buffer, s, recon_uv_stride);
				}
			}

			//V plane extend
			//left & right
			for (;v_buffer != v_end; v_buffer += recon_uv_stride)
			{
				unsigned char *s = v_buffer;
				unsigned char *d = s - Border;
				memset(d, *s, Border);
				d = s + xd->dst.uv_width;
				s = d-1;
				memset(d, *s, Border);
			}
			if (mb_row == 0)
			{
				//extend up
				v_end = dst_buffer[2] - Border;
				v_buffer = v_end - Border*recon_uv_stride;
				for (;v_buffer != v_end; v_buffer += recon_uv_stride)
				{
					memcpy(v_buffer, v_end, recon_uv_stride);
				}

				nProgress = 15 - lf_rows;
			}
			else if (mb_row >= pc->mb_rows-1)
			{
				//extend down
				const unsigned char * const s = v_end - Border - recon_uv_stride;
				v_buffer -= Border;
				v_end = v_buffer + Border*recon_uv_stride;
				for (;v_buffer != v_end; v_buffer += recon_uv_stride)
				{
					memcpy(v_buffer, s, recon_uv_stride);
				}

// 				nProgress = pc->Height + Border*2 - 1;
// 				nProgress = xd->dst.y_height + Border*2 - 1;
				nProgress = INT_MAX;
			}
			else
			{
				nProgress = 16*mb_row + 15 - lf_rows;
			}

			{
				//report progress
				pthread_mutex_lock(pbi->pThdInfo->progressMutex);
				pc->yv12_fb[dst_fb_idx].nProgress = nProgress;
				pthread_cond_broadcast(pbi->pThdInfo->progressCond);
#if USE_WIN32_BROADCAST
#if defined(VOWINCE) || defined(WIN32)
				{
					VO_U32 i;
					for (i = pbi->nThdNum-1; i > 0; --i)
					{
						pthread_cond_signal(pbi->pThdInfo->progressCond);
					}
				}
#endif
#endif
				pthread_mutex_unlock(pbi->pThdInfo->progressMutex);
			}	
		}
#endif
    }
}

static unsigned int read_partition_size(const unsigned char *cx_size)
{
    const unsigned int size =
        cx_size[0] + (cx_size[1] << 8) + (cx_size[2] << 16);
    return size;
}

static int read_is_valid(const unsigned char *start,
                         size_t               len,
                         const unsigned char *end)
{
    return (start + len > start && start + len <= end);
}

static unsigned int read_available_partition_size(
                                       VP8D_COMP *pbi,
                                       const unsigned char *token_part_sizes,
                                       const unsigned char *fragment_start,
                                       const unsigned char *first_fragment_end,
                                       const unsigned char *fragment_end,
                                       int i,
                                       int num_part)
{
    VP8_COMMON* pc = &pbi->common;
    const unsigned char *partition_size_ptr = token_part_sizes + i * 3;
    unsigned int partition_size = 0;
    ptrdiff_t bytes_left = fragment_end - fragment_start;
    /* Calculate the length of this partition. The last partition
     * size is implicit. If the partition size can't be read, then
     * either use the remaining data in the buffer (for EC mode)
     * or throw an error.
     */
    if (i < num_part - 1)
    {
        if (read_is_valid(partition_size_ptr, 3, first_fragment_end))
            partition_size = read_partition_size(partition_size_ptr);
        else if (pbi->ec_active)
            partition_size = bytes_left;
        else
            vpx_internal_error(&pc->error, VPX_CODEC_CORRUPT_FRAME,
                               "Truncated partition size data");
    }
    else
        partition_size = bytes_left;

    /* Validate the calculated partition length. If the buffer
     * described by the partition can't be fully read, then restrict
     * it to the portion that can be (for EC mode) or throw an error.
     */
    if (!read_is_valid(fragment_start, partition_size, fragment_end))
    {
        if (pbi->ec_active)
            partition_size = bytes_left;
        else
            vpx_internal_error(&pc->error, VPX_CODEC_CORRUPT_FRAME,
                               "Truncated packet or corrupt partition "
                               "%d length", i + 1);
    }
    return partition_size;
}


void setup_token_decoder(VP8D_COMP *pbi,
                                const unsigned char* token_part_sizes)
{
    vp8_reader *bool_decoder = &pbi->bc2;
    unsigned int partition_idx;
    unsigned int fragment_idx;
    int num_token_partitions;
    const unsigned char *first_fragment_end = pbi->fragments[0] +
                                          pbi->fragment_sizes[0];

    TOKEN_PARTITION multi_token_partition =
            (TOKEN_PARTITION)vp8_read_literal(&pbi->bc, 2);
    if (!vp8dx_bool_error(&pbi->bc))
        pbi->common.multi_token_partition = multi_token_partition;
    num_token_partitions = 1 << pbi->common.multi_token_partition;
    if (num_token_partitions > 1)
    {
        CHECK_MEM_ERROR(pbi->mbc, vpx_malloc(num_token_partitions *
                                             sizeof(vp8_reader)));
        bool_decoder = pbi->mbc;
    }

    /* Check for partitions within the fragments and unpack the fragments
     * so that each fragment pointer points to its corresponding partition. */
    for (fragment_idx = 0; fragment_idx < pbi->num_fragments; ++fragment_idx)
    {
        unsigned int fragment_size = pbi->fragment_sizes[fragment_idx];
        const unsigned char *fragment_end = pbi->fragments[fragment_idx] +
                                            fragment_size;
        /* Special case for handling the first partition since we have already
         * read its size. */
        if (fragment_idx == 0)
        {
            /* Size of first partition + token partition sizes element */
            ptrdiff_t ext_first_part_size = token_part_sizes -
                pbi->fragments[0] + 3 * (num_token_partitions - 1);
            fragment_size -= ext_first_part_size;
            if (fragment_size > 0)
            {
                pbi->fragment_sizes[0] = ext_first_part_size;
                /* The fragment contains an additional partition. Move to
                 * next. */
                fragment_idx++;
                pbi->fragments[fragment_idx] = pbi->fragments[0] +
                  pbi->fragment_sizes[0];
            }
        }
        /* Split the chunk into partitions read from the bitstream */
        while (fragment_size > 0)
        {
            ptrdiff_t partition_size = read_available_partition_size(
                                                 pbi,
                                                 token_part_sizes,
                                                 pbi->fragments[fragment_idx],
                                                 first_fragment_end,
                                                 fragment_end,
                                                 fragment_idx - 1,
                                                 num_token_partitions);
            pbi->fragment_sizes[fragment_idx] = partition_size;
            fragment_size -= partition_size;
            assert((int)fragment_idx <= num_token_partitions);
            if (fragment_size > 0)
            {
                /* The fragment contains an additional partition.
                 * Move to next. */
                fragment_idx++;
                pbi->fragments[fragment_idx] =
                    pbi->fragments[fragment_idx - 1] + partition_size;
            }
        }
    }

    pbi->num_fragments = num_token_partitions + 1;

    for (partition_idx = 1; partition_idx < pbi->num_fragments; ++partition_idx)
    {
        if (vp8dx_start_decode(bool_decoder,
                               pbi->fragments[partition_idx],
                               pbi->fragment_sizes[partition_idx]))
            vpx_internal_error(&pbi->common.error, VPX_CODEC_MEM_ERROR,
                               "Failed to allocate bool decoder %d",
                               partition_idx);

        bool_decoder++;
    }

// #if CONFIG_MULTITHREAD
//     /* Clamp number of decoder threads */
//     if (pbi->decoding_thread_count > num_token_partitions - 1)
//         pbi->decoding_thread_count = num_token_partitions - 1;
// #endif
}

static void stop_token_decoder(VP8D_COMP *pbi)
{
    VP8_COMMON *pc = &pbi->common;

    if (pc->multi_token_partition != ONE_PARTITION)
    {
        vpx_free(pbi->mbc);
        pbi->mbc = NULL;
    }
}

void init_frame(VP8D_COMP *pbi)
{
    VP8_COMMON *const pc = & pbi->common;
    MACROBLOCKD *const xd  = & pbi->mb;

    if (pc->frame_type == KEY_FRAME)
    {
        /* Various keyframe initializations */
        vpx_memcpy(pc->nfc.mvc, vp8_default_mv_context, sizeof(vp8_default_mv_context));

        vp8_init_mbmode_probs(pc);

        vp8_default_coef_probs(pc);
        vp8_kf_default_bmode_probs(pc->kf_bmode_prob);

        /* reset the segment feature data to 0 with delta coding (Default state). */
        vpx_memset(xd->segment_feature_data, 0, sizeof(xd->segment_feature_data));
        xd->mb_segement_abs_delta = SEGMENT_DELTADATA;

        /* reset the mode ref deltasa for loop filter */
        vpx_memset(xd->ref_lf_deltas, 0, sizeof(xd->ref_lf_deltas));
        vpx_memset(xd->mode_lf_deltas, 0, sizeof(xd->mode_lf_deltas));

        /* All buffers are implicitly updated on key frames. */
        pc->refresh_golden_frame = 1;
        pc->refresh_alt_ref_frame = 1;
        pc->copy_buffer_to_gf = 0;
        pc->copy_buffer_to_arf = 0;

        /* Note that Golden and Altref modes cannot be used on a key frame so
         * ref_frame_sign_bias[] is undefined and meaningless
         */
        pc->ref_frame_sign_bias[GOLDEN_FRAME] = 0;
        pc->ref_frame_sign_bias[ALTREF_FRAME] = 0;
    }
    else
    {
        if (!pc->use_bilinear_mc_filter)
            pc->mcomp_filter_type = SIXTAP;
        else
            pc->mcomp_filter_type = BILINEAR;

        /* To enable choice of different interploation filters */
        if (pc->mcomp_filter_type == SIXTAP)
        {
            xd->subpixel_predict        = vp8_sixtap_predict4x4;
            xd->subpixel_predict8x4     = vp8_sixtap_predict8x4;
            xd->subpixel_predict8x8     = vp8_sixtap_predict8x8;
            xd->subpixel_predict16x16   = vp8_sixtap_predict16x16;
        }
        else
        {
            xd->subpixel_predict        = vp8_bilinear_predict4x4;
            xd->subpixel_predict8x4     = vp8_bilinear_predict8x4;
            xd->subpixel_predict8x8     = vp8_bilinear_predict8x8;
            xd->subpixel_predict16x16   = vp8_bilinear_predict16x16;
        }

        if (pbi->decoded_key_frame && pbi->ec_enabled && !pbi->ec_active)
            pbi->ec_active = 1;
    }

    xd->left_context = &pc->left_context;
    xd->mode_info_context = pc->mi;
    xd->frame_type = pc->frame_type;
    xd->mode_info_context->mbmi.mode = DC_PRED;
    xd->mode_info_stride = pc->mode_info_stride;
    xd->corrupted = 0; /* init without corruption */

    xd->fullpixel_mask = 0xffffffff;
    if(pc->full_pixel)
        xd->fullpixel_mask = 0xfffffff8;

}

int get_free_fb (VP8D_COMP *pbi, VP8_COMMON *cm);

int vp8_prepare_decode(VP8D_COMP *pbi)
{
	vp8_reader *const bc = & pbi->bc;
	VP8_COMMON *const pc = & pbi->common;
	MACROBLOCKD *const xd  = & pbi->mb;
	const unsigned char *data = pbi->fragments[0];
	const unsigned char *data_end =  data + pbi->fragment_sizes[0];

	int i, j, k, l;
	const int *const mb_feature_data_bits = vp8_mb_feature_data_bits;
	int corrupt_tokens = 0;
	int prev_independent_partitions = pbi->independent_partitions;
	ptrdiff_t first_partition_length_in_bytes;

	/* start with no corruption of current frame */
	xd->corrupted = 0;

	if (data_end - data < 3)
	{
		if (!pbi->ec_active)
		{
			vpx_internal_error(&pc->error, VPX_CODEC_CORRUPT_FRAME,
				"Truncated packet");
		}

		/* Declare the missing frame as an inter frame since it will
		be handled as an inter frame when we have estimated its
		motion vectors. */
		pc->frame_type = INTER_FRAME;
		pc->version = 0;
		pc->show_frame = 1;
		first_partition_length_in_bytes = 0;
	}
	else
	{
		pc->frame_type = (FRAME_TYPE)(data[0] & 1);
		pc->version = (data[0] >> 1) & 7;
		pc->show_frame = (data[0] >> 4) & 1;
		first_partition_length_in_bytes =
			(data[0] | (data[1] << 8) | (data[2] << 16)) >> 5;

		if (!pbi->ec_active && (data + first_partition_length_in_bytes > data_end
			|| data + first_partition_length_in_bytes < data))
			vpx_internal_error(&pc->error, VPX_CODEC_CORRUPT_FRAME,
			"Truncated packet or corrupt partition 0 length");

		data += 3;

		vp8_setup_version(pc);

		if (pc->frame_type == KEY_FRAME)
		{
			const int Width = pc->Width;
			const int Height = pc->Height;
			int prev_mb_rows = pc->mb_rows;

			/* vet via sync code */
			/* When error concealment is enabled we should only check the sync
			* code if we have enough bits available
			*/
			if (!pbi->ec_active || data + 3 < data_end)
			{
				if (data[0] != 0x9d || data[1] != 0x01 || data[2] != 0x2a)
					vpx_internal_error(&pc->error, VPX_CODEC_UNSUP_BITSTREAM,
					"Invalid frame sync code");
			}

			/* If error concealment is enabled we should only parse the new size
			* if we have enough data. Otherwise we will end up with the wrong
			* size.
			*/
			if (!pbi->ec_active || data + 6 < data_end)
			{
				pc->Width = (data[3] | (data[4] << 8)) & 0x3fff;
				//                 pc->horiz_scale = data[4] >> 6;
				pc->Height = (data[5] | (data[6] << 8)) & 0x3fff;
				//                 pc->vert_scale = data[6] >> 6;
			}
			data += 7;

			if (Width != pc->Width  ||  Height != pc->Height)
			{
				if (pc->global_init_done)
				{
					//BA not support for new multi-core
					return -1;
				}
				
				if (pc->Width <= 0)
				{
					pc->Width = Width;
					vpx_internal_error(&pc->error, VPX_CODEC_CORRUPT_FRAME,
						"Invalid frame width");
				}

				if (pc->Height <= 0)
				{
					pc->Height = Height;
					vpx_internal_error(&pc->error, VPX_CODEC_CORRUPT_FRAME,
						"Invalid frame height");
				}

				if (vp8_alloc_frame_buffers(pc, pc->Width, pc->Height, pbi->nThdNum))
					vpx_internal_error(&pc->error, VPX_CODEC_MEM_ERROR,
					"Failed to allocate frame buffers");

#if CONFIG_ERROR_CONCEALMENT
				pbi->overlaps = NULL;
				if (pbi->ec_enabled)
				{
					if (vp8_alloc_overlap_lists(pbi))
						vpx_internal_error(&pc->error, VPX_CODEC_MEM_ERROR,
						"Failed to allocate overlap lists "
						"for error concealment");
				}
#endif
				pc->global_init_done = 1;

				
			}
			if (pc->priv_init_done == 0)
			{
				//do only once for every thread
				if (vp8_alloc_priv_buffer(pc, pc->Width, pc->Height))
					vpx_internal_error(&pc->error, VPX_CODEC_MEM_ERROR,
					"Failed to allocate priv buffers");

#if CONFIG_MULTITHREAD
				vp8mt_alloc_temp_buffers(pbi, pc->Width, prev_mb_rows);
#endif
				pc->priv_init_done = 1;
			}
		}
		else
		{
			if (pc->global_init_done && pc->priv_init_done == 0)
			{
				//do only once for every thread
				int prev_mb_rows = pc->mb_rows;

				if (vp8_alloc_priv_buffer(pc, pc->Width, pc->Height))
					vpx_internal_error(&pc->error, VPX_CODEC_MEM_ERROR,
					"Failed to allocate priv buffers");

#if CONFIG_MULTITHREAD
				vp8mt_alloc_temp_buffers(pbi, pc->Width, prev_mb_rows);
#endif
				pc->priv_init_done = 1;
			}
			
		}
	}

	if ((pbi->nThdNum<=1 && !pbi->decoded_key_frame && pc->frame_type != KEY_FRAME) ||
		pc->Width == 0 || pc->Height == 0)
	{
		return -1;
	}

	pc->new_fb_idx = get_free_fb (pbi, pc);
	if (pc->new_fb_idx < 0)
	{
		return -1;
	}
	
	pc->yv12_fb[pc->new_fb_idx].corrupted = 0;
	pc->yv12_fb[pc->new_fb_idx].nProgress = -100;

	init_frame(pbi);

	if (vp8dx_start_decode(bc, data, data_end - data))
		vpx_internal_error(&pc->error, VPX_CODEC_MEM_ERROR,
		"Failed to allocate bool decoder 0");
	if (pc->frame_type == KEY_FRAME) {
		pc->clr_type    = (YUV_TYPE)vp8_read_bit(bc);
		/*pc->clamp_type  = (CLAMP_TYPE)*/vp8_read_bit(bc);
	}

	/* Is segmentation enabled */
	xd->segmentation_enabled = (unsigned char)vp8_read_bit(bc);

	if (xd->segmentation_enabled)
	{
		/* Signal whether or not the segmentation map is being explicitly updated this frame. */
		xd->update_mb_segmentation_map = (unsigned char)vp8_read_bit(bc);
		xd->update_mb_segmentation_data = (unsigned char)vp8_read_bit(bc);

		if (xd->update_mb_segmentation_data)
		{
			xd->mb_segement_abs_delta = (unsigned char)vp8_read_bit(bc);

			vpx_memset(xd->segment_feature_data, 0, sizeof(xd->segment_feature_data));

			/* For each segmentation feature (Quant and loop filter level) */
			for (i = 0; i < MB_LVL_MAX; i++)
			{
				for (j = 0; j < MAX_MB_SEGMENTS; j++)
				{
					/* Frame level data */
					if (vp8_read_bit(bc))
					{
						xd->segment_feature_data[i][j] = (signed char)vp8_read_literal(bc, mb_feature_data_bits[i]);

						if (vp8_read_bit(bc))
							xd->segment_feature_data[i][j] = -xd->segment_feature_data[i][j];
					}
					else
						xd->segment_feature_data[i][j] = 0;
				}
			}
		}

		if (xd->update_mb_segmentation_map)
		{
			/* Which macro block level features are enabled */
			vpx_memset(xd->mb_segment_tree_probs, 255, sizeof(xd->mb_segment_tree_probs));

			/* Read the probs used to decode the segment id for each macro block. */
			for (i = 0; i < MB_FEATURE_TREE_PROBS; i++)
			{
				/* If not explicitly set value is defaulted to 255 by memset above */
				if (vp8_read_bit(bc))
					xd->mb_segment_tree_probs[i] = (vp8_prob)vp8_read_literal(bc, 8);
			}
		}
	}
	else
	{
		/* No segmentation updates on this frame */
		xd->update_mb_segmentation_map = 0;
		xd->update_mb_segmentation_data = 0;
	}

	/* Read the loop filter level and type */
	pc->filter_type = (LOOPFILTERTYPE) vp8_read_bit(bc);
	pc->filter_level = vp8_read_literal(bc, 6);
	pc->sharpness_level = vp8_read_literal(bc, 3);

	/* Read in loop filter deltas applied at the MB level based on mode or ref frame. */
	xd->mode_ref_lf_delta_update = 0;
	xd->mode_ref_lf_delta_enabled = (unsigned char)vp8_read_bit(bc);

	if (xd->mode_ref_lf_delta_enabled)
	{
		/* Do the deltas need to be updated */
		xd->mode_ref_lf_delta_update = (unsigned char)vp8_read_bit(bc);

		if (xd->mode_ref_lf_delta_update)
		{
			/* Send update */
			for (i = 0; i < MAX_REF_LF_DELTAS; i++)
			{
				if (vp8_read_bit(bc))
				{
					/*sign = vp8_read_bit( bc );*/
					xd->ref_lf_deltas[i] = (signed char)vp8_read_literal(bc, 6);

					if (vp8_read_bit(bc))        /* Apply sign */
						xd->ref_lf_deltas[i] = xd->ref_lf_deltas[i] * -1;
				}
			}

			/* Send update */
			for (i = 0; i < MAX_MODE_LF_DELTAS; i++)
			{
				if (vp8_read_bit(bc))
				{
					/*sign = vp8_read_bit( bc );*/
					xd->mode_lf_deltas[i] = (signed char)vp8_read_literal(bc, 6);

					if (vp8_read_bit(bc))        /* Apply sign */
						xd->mode_lf_deltas[i] = xd->mode_lf_deltas[i] * -1;
				}
			}
		}
	}

	setup_token_decoder(pbi, data + first_partition_length_in_bytes);

	xd->current_bc = &pbi->bc2;

	/* Read the default quantizers. */
	{
		int Q, q_update;

		Q = vp8_read_literal(bc, 7);  /* AC 1st order Q = default */
		pc->base_qindex = Q;
		q_update = 0;
		pc->y1dc_delta_q = get_delta_q(bc, pc->y1dc_delta_q, &q_update);
		pc->y2dc_delta_q = get_delta_q(bc, pc->y2dc_delta_q, &q_update);
		pc->y2ac_delta_q = get_delta_q(bc, pc->y2ac_delta_q, &q_update);
		pc->uvdc_delta_q = get_delta_q(bc, pc->uvdc_delta_q, &q_update);
		pc->uvac_delta_q = get_delta_q(bc, pc->uvac_delta_q, &q_update);

		if (q_update)
			vp8cx_init_de_quantizer(pbi);

		/* MB level dequantizer setup */
		vp8_mb_init_dequantizer(pbi, &pbi->mb);
	}

	/* Determine if the golden frame or ARF buffer should be updated and how.
	* For all non key frames the GF and ARF refresh flags and sign bias
	* flags must be set explicitly.
	*/
	if (pc->frame_type != KEY_FRAME)
	{
		/* Should the GF or ARF be updated from the current frame */
		pc->refresh_golden_frame = vp8_read_bit(bc);
#if CONFIG_ERROR_CONCEALMENT
		/* Assume we shouldn't refresh golden if the bit is missing */
		xd->corrupted |= vp8dx_bool_error(bc);
		if (pbi->ec_active && xd->corrupted)
			pc->refresh_golden_frame = 0;
#endif

		pc->refresh_alt_ref_frame = vp8_read_bit(bc);
#if CONFIG_ERROR_CONCEALMENT
		/* Assume we shouldn't refresh altref if the bit is missing */
		xd->corrupted |= vp8dx_bool_error(bc);
		if (pbi->ec_active && xd->corrupted)
			pc->refresh_alt_ref_frame = 0;
#endif

		/* Buffer to buffer copy flags. */
		pc->copy_buffer_to_gf = 0;

		if (!pc->refresh_golden_frame)
			pc->copy_buffer_to_gf = vp8_read_literal(bc, 2);

#if CONFIG_ERROR_CONCEALMENT
		/* Assume we shouldn't copy to the golden if the bit is missing */
		xd->corrupted |= vp8dx_bool_error(bc);
		if (pbi->ec_active && xd->corrupted)
			pc->copy_buffer_to_gf = 0;
#endif

		pc->copy_buffer_to_arf = 0;

		if (!pc->refresh_alt_ref_frame)
			pc->copy_buffer_to_arf = vp8_read_literal(bc, 2);

#if CONFIG_ERROR_CONCEALMENT
		/* Assume we shouldn't copy to the alt-ref if the bit is missing */
		xd->corrupted |= vp8dx_bool_error(bc);
		if (pbi->ec_active && xd->corrupted)
			pc->copy_buffer_to_arf = 0;
#endif


		pc->ref_frame_sign_bias[GOLDEN_FRAME] = vp8_read_bit(bc);
		pc->ref_frame_sign_bias[ALTREF_FRAME] = vp8_read_bit(bc);
	}

	pc->refresh_entropy_probs = vp8_read_bit(bc);
#if CONFIG_ERROR_CONCEALMENT
	/* Assume we shouldn't refresh the probabilities if the bit is
	* missing */
	xd->corrupted |= vp8dx_bool_error(bc);
	if (pbi->ec_active && xd->corrupted)
		pc->refresh_entropy_probs = 0;
#endif
	if (pc->refresh_entropy_probs == 0)
	{
		vpx_memcpy(&pc->lfc, &pc->nfc, sizeof(pc->nfc));
		pc->cfc = &pc->lfc;
	}
	else
	{
		pc->cfc = &pc->nfc;
	}

	pc->refresh_last_frame = pc->frame_type == KEY_FRAME  ||  vp8_read_bit(bc);

#if CONFIG_ERROR_CONCEALMENT
	/* Assume we should refresh the last frame if the bit is missing */
	xd->corrupted |= vp8dx_bool_error(bc);
	if (pbi->ec_active && xd->corrupted)
		pc->refresh_last_frame = 1;
#endif

	if (0)
	{
		FILE *z = fopen("decodestats.stt", "a");
		fprintf(z, "%6d F:%d,G:%d,A:%d,L:%d,Q:%d\n",
			pc->current_video_frame,
			pc->frame_type,
			pc->refresh_golden_frame,
			pc->refresh_alt_ref_frame,
			pc->refresh_last_frame,
			pc->base_qindex);
		fclose(z);
	}

#if USE_FRAME_THREAD
	if (pbi->nThdNum > 1)
	{
		//backup
		pc->alt_fb_idx_s = pc->alt_fb_idx;
		pc->gld_fb_idx_s = pc->gld_fb_idx;
		pc->lst_fb_idx_s = pc->lst_fb_idx;
		vpx_memcpy(pc->fb_idx_ref_cnt_s, pc->fb_idx_ref_cnt, sizeof(int)*pc->imageBufSize);

		//swap
		if (swap_frame_buffers (pc, 1))
		{
			pbi->common.error.error_code = VPX_CODEC_ERROR;
			pbi->common.error.setjmp = 0;
			pbi->num_fragments = 0;
			return -1;
		}
	}
#endif

	{
		pbi->independent_partitions = 1;

		/* read coef probability tree */
		for (i = 0; i < BLOCK_TYPES; i++)
			for (j = 0; j < COEF_BANDS; j++)
				for (k = 0; k < PREV_COEF_CONTEXTS; k++)
					for (l = 0; l < ENTROPY_NODES; l++)
					{

						vp8_prob *const p = pc->cfc->coef_probs [i][j][k] + l;

						if (vp8_read(bc, vp8_coef_update_probs [i][j][k][l]))
						{
							*p = (vp8_prob)vp8_read_literal(bc, 8);

						}
						if (k > 0 && *p != pc->cfc->coef_probs[i][j][k-1][l])
							pbi->independent_partitions = 0;

					}

		/* Read the mb_no_coeff_skip flag */
		pc->mb_no_coeff_skip = (int)vp8_read_bit(bc);
		vp8_decode_mode_mvs(pbi);
	}

	if (pc->show_frame)
		pc->current_video_frame++;

	return 0;
}

int vp8_decode_frame(VP8D_COMP *pbi)
{
    vp8_reader *const bc = & pbi->bc;
    VP8_COMMON *const pc = & pbi->common;
    MACROBLOCKD *const xd  = & pbi->mb;
    const unsigned char *data = pbi->fragments[0];
    const unsigned char *data_end =  data + pbi->fragment_sizes[0];

// 	int i, j, k, l;
    const int *const mb_feature_data_bits = vp8_mb_feature_data_bits;
    int corrupt_tokens = 0;
    int prev_independent_partitions = pbi->independent_partitions;

#if USE_FRAME_THREAD
	if (pbi->nThdNum > 1)
	{
		vpx_memcpy(&xd->pre, &pc->yv12_fb[pc->lst_fb_idx_s], sizeof(YV12_BUFFER_CONFIG));
	}
	else
#endif
	{
		vpx_memcpy(&xd->pre, &pc->yv12_fb[pc->lst_fb_idx], sizeof(YV12_BUFFER_CONFIG));
	}
    vpx_memcpy(&xd->dst, &pc->yv12_fb[pc->new_fb_idx], sizeof(YV12_BUFFER_CONFIG));

    /* set up frame new frame for intra coded blocks */
#if CONFIG_MULTITHREAD
//     if (/*!(pbi->b_multithreaded_rd) ||*/ pc->multi_token_partition == ONE_PARTITION || !(pc->filter_level))
#endif
        vp8_setup_intra_recon(&pc->yv12_fb[pc->new_fb_idx]);

    vp8_setup_block_dptrs(xd);

    vp8_build_block_doffsets(xd);

    /* clear out the coeff buffer */
    vpx_memset(xd->qcoeff, 0, sizeof(xd->qcoeff));




#if CONFIG_ERROR_CONCEALMENT
    if (pbi->ec_active &&
            pbi->mvs_corrupt_from_mb < (unsigned int)pc->mb_cols * pc->mb_rows)
    {
        /* Motion vectors are missing in this frame. We will try to estimate
         * them and then continue decoding the frame as usual */
        vp8_estimate_missing_mvs(pbi);
    }
#endif

    vpx_memset(pc->above_context, 0, sizeof(ENTROPY_CONTEXT_PLANES) * pc->mb_cols);
//     pbi->frame_corrupt_residual = 0;

// #if CONFIG_MULTITHREAD
//     if (pbi->b_multithreaded_rd && pc->multi_token_partition != ONE_PARTITION)
//     {
//         int i;
//         vp8mt_decode_mb_rows(pbi, xd);
//         vp8_yv12_extend_frame_borders(&pc->yv12_fb[pc->new_fb_idx]);    /*cm->frame_to_show);*/
//         for (i = 0; i < pbi->decoding_thread_count; ++i)
//             corrupt_tokens |= pbi->mb_row_di[i].mbd.corrupted;
//     }
//     else
// #endif
    {
        decode_mb_rows(pbi);
        corrupt_tokens |= xd->corrupted;
    }

    stop_token_decoder(pbi);

    /* Collect information about decoder corruption. */
    /* 1. Check first boolean decoder for errors. */
    pc->yv12_fb[pc->new_fb_idx].corrupted = vp8dx_bool_error(bc);
    /* 2. Check the macroblock information */
    pc->yv12_fb[pc->new_fb_idx].corrupted |= corrupt_tokens;

    if (!pbi->decoded_key_frame)
    {
        if (pc->frame_type == KEY_FRAME &&
            !pc->yv12_fb[pc->new_fb_idx].corrupted)
            pbi->decoded_key_frame = 1;
//         else
//             vpx_internal_error(&pbi->common.error, VPX_CODEC_CORRUPT_FRAME,
//                                "A stream must start with a complete key frame");
    }

    /* vpx_log("Decoder: Frame Decoded, Size Roughly:%d bytes  \n",bc->pos+pbi->bc2.pos); */

    /* If this was a kf or Gf note the Q used */
    if ((pc->frame_type == KEY_FRAME) ||
         pc->refresh_golden_frame || pc->refresh_alt_ref_frame)
    {
//         pc->last_kf_gf_q = pc->base_qindex;
    }

    if (pc->refresh_entropy_probs == 0)
    {
//         vpx_memcpy(&pc->fc, &pc->lfc, sizeof(pc->fc));
        pbi->independent_partitions = prev_independent_partitions;
    }

#ifdef PACKET_TESTING
    {
        FILE *f = fopen("decompressor.VP8", "ab");
        unsigned int size = pbi->bc2.pos + pbi->bc.pos + 8;
        fwrite((void *) &size, 4, 1, f);
        fwrite((void *) pbi->Source, size, 1, f);
        fclose(f);
    }
#endif

    return 0;
}
