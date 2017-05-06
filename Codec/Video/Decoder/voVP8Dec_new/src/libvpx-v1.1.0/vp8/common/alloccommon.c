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
#include "blockd.h"
#include "vpx_mem/vpx_mem.h"
#include "onyxc_int.h"
#include "findnearmv.h"
#include "entropymode.h"
#include "systemdependent.h"


extern  void vp8_init_scan_order_mask();
extern int FrameBufCtl(FIFOTYPE *priv, int img_index , const int flag);

static void update_mode_info_border(MODE_INFO *mi, int rows, int cols)
{
    int i;
    vpx_memset(mi - cols - 2, 0, sizeof(MODE_INFO) * (cols + 1));

    for (i = 0; i < rows; i++)
    {
        /* TODO(holmer): Bug? This updates the last element of each row
         * rather than the border element!
         */
        vpx_memset(&mi[i*cols-1], 0, sizeof(MODE_INFO));
    }
}

void vp8_de_alloc_frame_buffers(VP8_COMMON *oci)
{
    int i;
	if (oci->yv12_fb)
	{
		for (i = 0; i < oci->imageBufSize; i++)
			vp8_yv12_de_alloc_frame_buffer(&oci->yv12_fb[i]);

		vpx_free(oci->yv12_fb);
	}
	
	vpx_free(oci->fb_idx_ref_cnt);
#if USE_FRAME_THREAD
	vpx_free(oci->p_output);
	vpx_free(oci->p_remove);
#endif	
	vpx_free(oci->pSegID_s);
#if CONFIG_POSTPROC
    vp8_yv12_de_alloc_frame_buffer(&oci->post_proc_buffer);
    if (oci->post_proc_buffer_int_used)
        vp8_yv12_de_alloc_frame_buffer(&oci->post_proc_buffer_int);
#endif
}

int vp8_alloc_frame_buffers(VP8_COMMON *oci, int width, int height, int nThd)
{
    int i;

    vp8_de_alloc_frame_buffers(oci);

    /* our internal buffers are always multiples of 16 */
    if ((width & 0xf) != 0)
        width += 16 - (width & 0xf);

    if ((height & 0xf) != 0)
        height += 16 - (height & 0xf);
	oci->mb_rows = height >> 4;
	oci->mb_cols = width >> 4;

	if (nThd > 1)
	{
		oci->pSegID_s = (int *)vpx_calloc((oci->mb_cols + 1) * (oci->mb_rows + 1), sizeof(int));
		if (oci->pSegID_s == NULL)
		{
			return 1;
		}
		oci->imageBufSize = NUM_YV12_BUFFERS + nThd*2;
	}
	else
	{
		oci->pSegID_s = NULL;
		oci->imageBufSize = NUM_YV12_BUFFERS + 2;
	}
	
#if USE_FRAME_THREAD
	oci->p_output = (YV12_BUFFER_CONFIG **)vpx_calloc(oci->imageBufSize, sizeof(YV12_BUFFER_CONFIG *));
	if (oci->p_output == NULL)
	{
		vp8_de_alloc_frame_buffers(oci);
		return 1;
	}
	oci->output_size = 0;
	oci->p_remove = (YV12_BUFFER_CONFIG **)vpx_calloc(oci->imageBufSize, sizeof(YV12_BUFFER_CONFIG *));
	if (oci->p_remove == NULL)
	{
		vp8_de_alloc_frame_buffers(oci);
		return 1;
	}
	oci->remove_size = 0;
#endif
	oci->fb_idx_ref_cnt = (int*)vpx_calloc(oci->imageBufSize, sizeof(int));
	oci->yv12_fb = (YV12_BUFFER_CONFIG *)vpx_calloc(oci->imageBufSize, sizeof(YV12_BUFFER_CONFIG));
	if (oci->fb_idx_ref_cnt == NULL || oci->yv12_fb == NULL)
	{
		vp8_de_alloc_frame_buffers(oci);
		return 1;
	}
	
	oci->vFrameIndexFIFO.w_idx = oci->vFrameIndexFIFO.r_idx = 0;
    for (i = 0; i < oci->imageBufSize; i++)
    {
        oci->fb_idx_ref_cnt[i] = 0;
        oci->yv12_fb[i].flags = 0;
        if (vp8_yv12_alloc_frame_buffer(&oci->yv12_fb[i], width, height, VP8BORDERINPIXELS) < 0)
        {
            vp8_de_alloc_frame_buffers(oci);
            return 1;
        }
		FrameBufCtl(&oci->vFrameIndexFIFO, i, FIFO_WRITE);
    }

    oci->new_fb_idx = 0;
    oci->lst_fb_idx = 1;
    oci->gld_fb_idx = 2;
    oci->alt_fb_idx = 3;

//     oci->fb_idx_ref_cnt[0] = 1;
//     oci->fb_idx_ref_cnt[1] = 1;
//     oci->fb_idx_ref_cnt[2] = 1;
//     oci->fb_idx_ref_cnt[3] = 1;

#if CONFIG_POSTPROC
    if (vp8_yv12_alloc_frame_buffer(&oci->post_proc_buffer, width, height, VP8BORDERINPIXELS) < 0)
    {
        vp8_de_alloc_frame_buffers(oci);
        return 1;
    }

    oci->post_proc_buffer_int_used = 0;
    vpx_memset(&oci->postproc_state, 0, sizeof(oci->postproc_state));
    vpx_memset((&oci->post_proc_buffer)->buffer_alloc,128,(&oci->post_proc_buffer)->frame_size);
#endif


    return 0;
}

void vp8_de_alloc_priv_buffer(VP8_COMMON *oci)
{
	vpx_free(oci->above_context);
	vpx_free(oci->mip);

	oci->above_context = 0;
	oci->mip = 0;

#if USE_FRAME_THREAD
	vpx_free(oci->fb_idx_ref_cnt_s);
	oci->fb_idx_ref_cnt_s = 0;
#endif

#if CONFIG_ERROR_CONCEALMENT
	vpx_free(oci->prev_mip);
	oci->prev_mip = 0;
#endif
}

int vp8_alloc_priv_buffer(VP8_COMMON *oci, int width, int height)
{
	vp8_de_alloc_priv_buffer(oci);

	/* our internal buffers are always multiples of 16 */
	if ((width & 0xf) != 0)
		width += 16 - (width & 0xf);

	if ((height & 0xf) != 0)
		height += 16 - (height & 0xf);
#if USE_FRAME_THREAD
	oci->fb_idx_ref_cnt_s = (int*)vpx_calloc(oci->imageBufSize, sizeof(int));
	if (oci->fb_idx_ref_cnt_s == NULL)
	{
		return 1;
	}
#endif

//     oci->MBs = oci->mb_rows * oci->mb_cols;
	oci->mode_info_stride = oci->mb_cols + 1;
	oci->mip = vpx_calloc((oci->mb_cols + 1) * (oci->mb_rows + 1), sizeof(MODE_INFO));

	if (!oci->mip)
	{
// 		vp8_de_alloc_priv_buffer(oci);
		return 1;
	}

	oci->mi = oci->mip + oci->mode_info_stride + 1;

	/* allocate memory for last frame MODE_INFO array */
#if CONFIG_ERROR_CONCEALMENT
	oci->prev_mip = vpx_calloc((oci->mb_cols + 1) * (oci->mb_rows + 1), sizeof(MODE_INFO));

	if (!oci->prev_mip)
	{
		vp8_de_alloc_frame_buffers(oci);
		return 1;
	}

	oci->prev_mi = oci->prev_mip + oci->mode_info_stride + 1;
#else
	oci->prev_mip = NULL;
	oci->prev_mi = NULL;
#endif

	oci->above_context = vpx_calloc(sizeof(ENTROPY_CONTEXT_PLANES) * oci->mb_cols, 1);

	if (!oci->above_context)
	{
		vp8_de_alloc_priv_buffer(oci);
		return 1;
	}

	update_mode_info_border(oci->mi, oci->mb_rows, oci->mb_cols);
#if CONFIG_ERROR_CONCEALMENT
	update_mode_info_border(oci->prev_mi, oci->mb_rows, oci->mb_cols);
#endif

	return 0;
}

void vp8_setup_version(VP8_COMMON *cm)
{
    switch (cm->version)
    {
    case 0:
        cm->no_lpf = 0;
        cm->filter_type = NORMAL_LOOPFILTER;
        cm->use_bilinear_mc_filter = 0;
        cm->full_pixel = 0;
        break;
    case 1:
        cm->no_lpf = 0;
        cm->filter_type = SIMPLE_LOOPFILTER;
        cm->use_bilinear_mc_filter = 1;
        cm->full_pixel = 0;
        break;
    case 2:
        cm->no_lpf = 1;
        cm->filter_type = NORMAL_LOOPFILTER;
        cm->use_bilinear_mc_filter = 1;
        cm->full_pixel = 0;
        break;
    case 3:
        cm->no_lpf = 1;
        cm->filter_type = SIMPLE_LOOPFILTER;
        cm->use_bilinear_mc_filter = 1;
        cm->full_pixel = 1;
        break;
    default:
        /*4,5,6,7 are reserved for future use*/
        cm->no_lpf = 0;
        cm->filter_type = NORMAL_LOOPFILTER;
        cm->use_bilinear_mc_filter = 0;
        cm->full_pixel = 0;
        break;
    }
}
void vp8_create_common(VP8_COMMON *oci)
{
    vp8_machine_specific_config(oci);

    vp8_init_mbmode_probs(oci);
    vp8_default_bmode_probs(oci->nfc.bmode_prob);

    oci->mb_no_coeff_skip = 1;
    oci->no_lpf = 0;
    oci->filter_type = NORMAL_LOOPFILTER;
    oci->use_bilinear_mc_filter = 0;
    oci->full_pixel = 0;
    oci->multi_token_partition = ONE_PARTITION;
    oci->clr_type = REG_YUV;
//     oci->clamp_type = RECON_CLAMP_REQUIRED;

    /* Initialize reference frame sign bias structure to defaults */
    vpx_memset(oci->ref_frame_sign_bias, 0, sizeof(oci->ref_frame_sign_bias));

    /* Default disable buffer to buffer copying */
    oci->copy_buffer_to_gf = 0;
    oci->copy_buffer_to_arf = 0;
}

void vp8_remove_common(VP8_COMMON *oci)
{
    vp8_de_alloc_frame_buffers(oci);
}
