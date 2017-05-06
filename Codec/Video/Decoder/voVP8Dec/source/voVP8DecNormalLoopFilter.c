#include "voVP8GlobleDef.h"
#include "voVP8LoopFilter.h"
#include "voVP8DecFrame.h"
#include "voVP8Common.h"
//**********************************************************************

void vp8_loop_filter_mbh(unsigned char *y_ptr, unsigned char *u_ptr, unsigned char *v_ptr,
                           int y_stride, int uv_stride, loop_filter_info *lfi, int simpler_lpf)
{ 
	(void) simpler_lpf;
#ifdef VOARMV7
	vp8_mbloop_filter_horizontal_edge_y_neon(y_ptr, y_stride, lfi->mbflim, lfi->lim, lfi->thr, 2);
	vp8_mbloop_filter_horizontal_edge_uv_neon(u_ptr, uv_stride, lfi->mbflim, lfi->lim, lfi->thr, v_ptr);
#elif VOARMV6
	vp8_mbloop_filter_horizontal_edge_armv6(y_ptr, y_stride, lfi->mbflim, lfi->lim, lfi->thr, 2);
    vp8_mbloop_filter_horizontal_edge_armv6(u_ptr, uv_stride, lfi->mbflim, lfi->lim, lfi->thr, 1);
    vp8_mbloop_filter_horizontal_edge_armv6(v_ptr, uv_stride, lfi->mbflim, lfi->lim, lfi->thr, 1);
#else
    vp8_mbloop_filter_horizontal_edge_c(y_ptr, y_stride, lfi->mbflim, lfi->lim, lfi->thr, 2);
    vp8_mbloop_filter_horizontal_edge_c(u_ptr, uv_stride, lfi->mbflim, lfi->lim, lfi->thr, 1);
    vp8_mbloop_filter_horizontal_edge_c(v_ptr, uv_stride, lfi->mbflim, lfi->lim, lfi->thr, 1);
#endif
}

 void vp8_loop_filter_mbv(unsigned char *y_ptr, unsigned char *u_ptr, unsigned char *v_ptr,
                           int y_stride, int uv_stride, loop_filter_info *lfi, int simpler_lpf)
{
	(void) simpler_lpf;
#ifdef VOARMV7
    vp8_mbloop_filter_vertical_edge_y_neon(y_ptr, y_stride, lfi->mbflim, lfi->lim, lfi->thr, 2);
	vp8_mbloop_filter_vertical_edge_uv_neon(u_ptr, uv_stride, lfi->mbflim, lfi->lim, lfi->thr, v_ptr);
#elif VOARMV6
	vp8_mbloop_filter_vertical_edge_armv6(y_ptr, y_stride, lfi->mbflim, lfi->lim, lfi->thr, 2);
	vp8_mbloop_filter_vertical_edge_armv6(u_ptr, uv_stride, lfi->mbflim, lfi->lim, lfi->thr, 1);
	vp8_mbloop_filter_vertical_edge_armv6(v_ptr, uv_stride, lfi->mbflim, lfi->lim, lfi->thr, 1);
#else
    vp8_mbloop_filter_vertical_edge_c(y_ptr, y_stride, lfi->mbflim, lfi->lim, lfi->thr, 2);
	vp8_mbloop_filter_vertical_edge_c(u_ptr, uv_stride, lfi->mbflim, lfi->lim, lfi->thr, 1);
	vp8_mbloop_filter_vertical_edge_c(v_ptr, uv_stride, lfi->mbflim, lfi->lim, lfi->thr, 1);
#endif
}

// Horizontal B Filtering
void vp8_loop_filter_bh(unsigned char *y_ptr, unsigned char *u_ptr, unsigned char *v_ptr,
                          int y_stride, int uv_stride, loop_filter_info *lfi, int simpler_lpf)
{
    (void) simpler_lpf;
#ifdef VOARMV7
	vp8_loop_filter_horizontal_edge_y_neon(y_ptr + 4 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
	vp8_loop_filter_horizontal_edge_y_neon(y_ptr + 8 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
	vp8_loop_filter_horizontal_edge_y_neon(y_ptr + 12 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_horizontal_edge_uv_neon(u_ptr + 4 * uv_stride, uv_stride, lfi->flim, lfi->lim, lfi->thr, v_ptr + 4 * uv_stride);
#elif VOARMV6
	vp8_loop_filter_horizontal_edge_armv6(y_ptr + 4 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_horizontal_edge_armv6(y_ptr + 8 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_horizontal_edge_armv6(y_ptr + 12 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
	vp8_loop_filter_horizontal_edge_armv6(u_ptr + 4 * uv_stride, uv_stride, lfi->flim, lfi->lim, lfi->thr, 1);
	vp8_loop_filter_horizontal_edge_armv6(v_ptr + 4 * uv_stride, uv_stride, lfi->flim, lfi->lim, lfi->thr, 1);
#else
    vp8_loop_filter_horizontal_edge_c(y_ptr + 4 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_horizontal_edge_c(y_ptr + 8 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_horizontal_edge_c(y_ptr + 12 * y_stride, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
	vp8_loop_filter_horizontal_edge_c(u_ptr + 4 * uv_stride, uv_stride, lfi->flim, lfi->lim, lfi->thr, 1);
	vp8_loop_filter_horizontal_edge_c(v_ptr + 4 * uv_stride, uv_stride, lfi->flim, lfi->lim, lfi->thr, 1);
#endif
}
// Vertical B Filtering
void vp8_loop_filter_bv(unsigned char *y_ptr, unsigned char *u_ptr, unsigned char *v_ptr,
                          int y_stride, int uv_stride, loop_filter_info *lfi, int simpler_lpf)
{
    (void) simpler_lpf;
#ifdef VOARMV7
	vp8_loop_filter_vertical_edge_y_neon(y_ptr + 4, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
	vp8_loop_filter_vertical_edge_y_neon(y_ptr + 8, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
	vp8_loop_filter_vertical_edge_y_neon(y_ptr + 12, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_vertical_edge_uv_neon(u_ptr + 4, uv_stride, lfi->flim, lfi->lim, lfi->thr, v_ptr + 4);
#elif VOARMV6
	vp8_loop_filter_vertical_edge_armv6(y_ptr + 4, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_vertical_edge_armv6(y_ptr + 8, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_vertical_edge_armv6(y_ptr + 12, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_vertical_edge_armv6(u_ptr + 4, uv_stride, lfi->flim, lfi->lim, lfi->thr, 1);
    vp8_loop_filter_vertical_edge_armv6(v_ptr + 4, uv_stride, lfi->flim, lfi->lim, lfi->thr, 1);
#else
    vp8_loop_filter_vertical_edge_c(y_ptr + 4, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_vertical_edge_c(y_ptr + 8, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_vertical_edge_c(y_ptr + 12, y_stride, lfi->flim, lfi->lim, lfi->thr, 2);
    vp8_loop_filter_vertical_edge_c(u_ptr + 4, uv_stride, lfi->flim, lfi->lim, lfi->thr, 1);
    vp8_loop_filter_vertical_edge_c(v_ptr + 4, uv_stride, lfi->flim, lfi->lim, lfi->thr, 1);
#endif
}

void vp8_init_loop_filter(VP8_COMMON *cm)
{
    loop_filter_info *lfi = cm->lf_info;
    LOOPFILTERTYPE lft = cm->filter_type;
    int sharpness_lvl = cm->sharpness_level;
    int frame_type = cm->frame_type;
    int i, j;

    int block_inside_limit = 0;
    int HEVThresh;
    const int yhedge_boost  = 2;
    //const int uvhedge_boost = 2;

    // For each possible value for the loop filter fill out a "loop_filter_info" entry.
    for (i = 0; i <= MAX_LOOP_FILTER; i++)
    {
        int filt_lvl = i;

        if (frame_type == KEY_FRAME)
        {
            if (filt_lvl >= 40)
                HEVThresh = 2;
            else if (filt_lvl >= 15)
                HEVThresh = 1;
            else
                HEVThresh = 0;
        }
        else
        {
            if (filt_lvl >= 40)
                HEVThresh = 3;
            else if (filt_lvl >= 20)
                HEVThresh = 2;
            else if (filt_lvl >= 15)
                HEVThresh = 1;
            else
                HEVThresh = 0;
        }

        // Set loop filter paramaeters that control sharpness.
        block_inside_limit = filt_lvl >> (sharpness_lvl > 0);
        block_inside_limit = block_inside_limit >> (sharpness_lvl > 4);

        if (sharpness_lvl > 0)
        {
            if (block_inside_limit > (9 - sharpness_lvl))
                block_inside_limit = (9 - sharpness_lvl);
        }

        if (block_inside_limit < 1)
            block_inside_limit = 1;

        for (j = 0; j < 16; j++)
        {
            lfi[i].lim[j] = block_inside_limit;
            lfi[i].mbflim[j] =(filt_lvl + yhedge_boost)*2+block_inside_limit; //flimit * 2 + limit
            lfi[i].flim[j] =filt_lvl*2+block_inside_limit;
            lfi[i].thr[j] = HEVThresh;
        }
    }

    // Set up the function pointers depending on the type of loop filtering selected
    if (lft == NORMAL_LOOPFILTER)
    {
		cm->lf_mbv = vp8_loop_filter_mbv;
        cm->lf_bv   = vp8_loop_filter_bv;
        cm->lf_mbh = vp8_loop_filter_mbh;
		//cm->lf_mbvh = NULL;//LF_INVOKE(&cm->rtcd.loopfilter, normal_mb_vh); //zou no usefull
        cm->lf_bh  = vp8_loop_filter_bh;
    }
    else
    {
        cm->lf_mbv = vp8_loop_filter_mbvs;
        cm->lf_bv   = vp8_loop_filter_bvs;
        cm->lf_mbh = vp8_loop_filter_mbhs; 
		//cm->lf_mbvh = LF_INVOKE(&cm->rtcd.loopfilter, simple_mb_vh);
        cm->lf_bh     = vp8_loop_filter_bhs;
    }
}

void vp8_frame_init_loop_filter(loop_filter_info *lfi, int frame_type)
{
    int HEVThresh;
    int i, j;

    // For each possible value for the loop filter fill out a "loop_filter_info" entry.
    for (i = 0; i <= MAX_LOOP_FILTER; i++)
    {
        int filt_lvl = i;

        if (frame_type == KEY_FRAME)
        {
            if (filt_lvl >= 40)
                HEVThresh = 2;
            else if (filt_lvl >= 15)
                HEVThresh = 1;
            else
                HEVThresh = 0;
        }
        else
        {
            if (filt_lvl >= 40)
                HEVThresh = 3;
            else if (filt_lvl >= 20)
                HEVThresh = 2;
            else if (filt_lvl >= 15)
                HEVThresh = 1;
            else
                HEVThresh = 0;
        }

        for (j = 0; j < 16; j++)
        {
            lfi[i].thr[j] = HEVThresh;
        }
    }
}

void vp8_adjust_mb_lf_value(MACROBLOCKD *mbd, int *filter_level)
{
    MB_MODE_INFO *mbmi = &mbd->mode_info_context->mbmi;

    if (mbd->mode_ref_lf_delta_enabled)
    {
        // Aplly delta for reference frame
        *filter_level += mbd->ref_lf_deltas[mbmi->ref_frame];

        // Apply delta for mode
        if (mbmi->ref_frame == INTRA_FRAME)
        {
            // Only the split mode BPRED has a further special case
            if (mbmi->mode == B_PRED)
                *filter_level +=  mbd->mode_lf_deltas[0];
        }
        else
        {
            // Zero motion mode
            if (mbmi->mode == ZEROMV)
                *filter_level +=  mbd->mode_lf_deltas[1];

            // Split MB motion mode
            else if (mbmi->mode == SPLITMV)
                *filter_level +=  mbd->mode_lf_deltas[3];

            // All other inter motion modes (Nearest, Near, New)
            else
                *filter_level +=  mbd->mode_lf_deltas[2];
        }

        // Range check
        if (*filter_level > MAX_LOOP_FILTER)
            *filter_level = MAX_LOOP_FILTER;
        else if (*filter_level < 0)
            *filter_level = 0;
    }
}
int loopfilter_mbrow(VP8D_COMP *pbi, VP8_COMMON *cm,MACROBLOCKD *mbd,int *baseline_filter_level,
					 unsigned char*y_ptr, unsigned char*u_ptr, unsigned char*v_ptr,int mb_row,int thread)
{
	int mb_col =0;
	int filter_level;
	int alt_flt_enabled = mbd->segmentation_enabled;
	YV12_BUFFER_CONFIG *post = cm->frame_to_show;
	loop_filter_info *lfi = cm->lf_info;
	MODE_INFO* pModeInfo = mbd->mode_info_context + mb_row*(cm->mb_cols+1); //+1 Skip border mb

	pbi->thread_mb_row[thread] = mb_row;
	pbi->thread_mb_col[thread]  = 0;
	for (mb_col = 0; mb_col < cm->mb_cols; mb_col++)
    {
        int Segment = (alt_flt_enabled) ? pModeInfo->mbmi.segment_id : 0;

		if(pbi->max_threads == 2)
		{
			volatile int *mby = &pbi->thread_mb_row[thread^1];
			volatile int *mbx = &pbi->thread_mb_col[thread^1];	
			while(mb_col >= *mbx && mb_row > *mby && mb_row>0)
			{
				;
			} 
		}

#ifndef FILTER_LEVEL
        filter_level = baseline_filter_level[Segment];
        vp8_adjust_mb_lf_value(mbd, &filter_level);
#else
		filter_level = pModeInfo->filter_level +  baseline_filter_level[Segment];

		if (filter_level > MAX_LOOP_FILTER)
			filter_level = MAX_LOOP_FILTER;
		else if (filter_level < 0)
			filter_level = 0;
#endif

		if (filter_level)
		{
			if (pModeInfo->mbmi.dc_diff > 0)
			{
				if (mb_col > 0)
				{
					cm->lf_mbv(y_ptr, u_ptr, v_ptr, post->y_stride, post->uv_stride, &lfi[filter_level], cm->simpler_lpf);
					cm->lf_bv  (y_ptr, u_ptr, v_ptr, post->y_stride, post->uv_stride, &lfi[filter_level], cm->simpler_lpf);
				}
				else
					cm->lf_bv(y_ptr, u_ptr, v_ptr, post->y_stride, post->uv_stride, &lfi[filter_level], cm->simpler_lpf);

				if (mb_row > 0)
				{
					cm->lf_mbh(y_ptr, u_ptr, v_ptr, post->y_stride, post->uv_stride, &lfi[filter_level], cm->simpler_lpf);
					cm->lf_bh  (y_ptr, u_ptr, v_ptr, post->y_stride, post->uv_stride, &lfi[filter_level], cm->simpler_lpf);
				}
				else
					cm->lf_bh(y_ptr, u_ptr, v_ptr, post->y_stride, post->uv_stride, &lfi[filter_level], cm->simpler_lpf);
			}
			else
			{
				if (mb_col > 0&&mb_row > 0)
				{						
					cm->lf_mbv(y_ptr, u_ptr, v_ptr, post->y_stride, post->uv_stride, &lfi[filter_level], cm->simpler_lpf);
					cm->lf_mbh(y_ptr, u_ptr, v_ptr, post->y_stride, post->uv_stride, &lfi[filter_level], cm->simpler_lpf);
				}
				else
				{
					if (mb_col > 0)
						cm->lf_mbv(y_ptr, u_ptr, v_ptr, post->y_stride, post->uv_stride, &lfi[filter_level], cm->simpler_lpf);
					if (mb_row > 0)// vp8_loop_filter_mbh_neon
						cm->lf_mbh(y_ptr, u_ptr, v_ptr, post->y_stride, post->uv_stride, &lfi[filter_level], cm->simpler_lpf);
				}
			}			
		}

        y_ptr += 16;
        u_ptr += 8;
        v_ptr += 8;
        pModeInfo++;     // step to next MB

		pbi->thread_mb_col[thread]  = mb_col;
    }

	return 1;
}
void vp8_loop_filter_frame( VP8D_COMP *pbi, VP8_COMMON *cm, MACROBLOCKD *mbd, int default_filt_lvl)
{
    YV12_BUFFER_CONFIG *post = cm->frame_to_show;
    loop_filter_info *lfi = cm->lf_info;
    int frame_type = cm->frame_type;

    int mb_row;

    //int baseline_filter_level[MAX_MB_SEGMENTS];
    int alt_flt_enabled = mbd->segmentation_enabled;

    int i;
    unsigned char *y_ptr, *u_ptr, *v_ptr;

    mbd->mode_info_context = cm->mi;          // Point at base of Mb MODE_INFO list

    // Note the baseline filter values for each segment
    if (alt_flt_enabled)
    {
        for (i = 0; i < MAX_MB_SEGMENTS; i++)
        {
            // Abs value
            if (mbd->mb_segement_abs_delta == SEGMENT_ABSDATA)
                pbi->baseline_filter_level[i] = mbd->segment_feature_data[MB_LVL_ALT_LF][i];
            // Delta Value
            else
            {
                pbi->baseline_filter_level[i] = default_filt_lvl + mbd->segment_feature_data[MB_LVL_ALT_LF][i];
                pbi->baseline_filter_level[i] = (pbi->baseline_filter_level[i] >= 0) ? ((pbi->baseline_filter_level[i] <= MAX_LOOP_FILTER) ? pbi->baseline_filter_level[i] : MAX_LOOP_FILTER) : 0;  // Clamp to valid range
            }
        }
    }
    else
    {
        for (i = 0; i < MAX_MB_SEGMENTS; i++)
            pbi->baseline_filter_level[i] = default_filt_lvl;
    }

    // Initialize the loop filter for this frame.
    if ((cm->last_filter_type != cm->filter_type) || (cm->last_sharpness_level != cm->sharpness_level))
        vp8_init_loop_filter(cm);
    else if (frame_type != cm->last_frame_type)
        vp8_frame_init_loop_filter(lfi, frame_type);

    // Set up the buffer pointers
    y_ptr = post->y_buffer;
    u_ptr = post->u_buffer;
    v_ptr = post->v_buffer;

	if(pbi->max_threads  == 2)
	{
		pbi->thread_mb_row[0] = 0;
        pbi->thread_mb_col[0] = 0;
		pbi->thread_mb_row[1] = 0;
        pbi->thread_mb_col[1] = 0;
		pbi->isloopfilter = 1;
		sem_post(pbi->h_event_startframe);

		for (mb_row = 0; mb_row < cm->mb_rows; mb_row+=2)
		{
			//printf("main ----row %d \n",mb_row);
			y_ptr = post->y_buffer+mb_row*post->y_stride  * 16;
			u_ptr = post->u_buffer+mb_row*post->uv_stride  * 8;
			v_ptr = post->v_buffer+mb_row*post->uv_stride  * 8;
		 
			loopfilter_mbrow(pbi,cm, mbd, pbi->baseline_filter_level,
						 y_ptr, u_ptr, v_ptr, mb_row,0);

		}
		pbi->thread_mb_row[0] = cm->mb_rows+1;

		if(pbi->max_threads == 2)
			sem_wait(pbi->h_event_main);

		while(!sem_trywait(pbi->h_event_main));
		while(!sem_trywait(pbi->h_event_startframe));

		pbi->isloopfilter = 0;

	}
	else
	{
		for (mb_row = 0; mb_row < cm->mb_rows; mb_row++)
		{
			y_ptr = post->y_buffer+mb_row*post->y_stride  * 16;
			u_ptr = post->u_buffer+mb_row*post->uv_stride  * 8;
			v_ptr = post->v_buffer+mb_row*post->uv_stride  * 8;

			loopfilter_mbrow(pbi, cm, mbd, pbi->baseline_filter_level,
						 y_ptr, u_ptr, v_ptr, mb_row,0);

		} 
	}
}  