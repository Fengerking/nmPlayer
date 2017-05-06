#include "voVP8DecReadMode.h"

void vp8_read_mb_features(vp8_reader *r, MB_MODE_INFO *mi, MACROBLOCKD *x)
{
    // Is segmentation enabled
    if (x->segmentation_enabled && x->update_mb_segmentation_map)
    {
        // If so then read the segment id.
        if (vp8_read(r, x->mb_segment_tree_probs[0]))
            mi->segment_id = (unsigned char)(2 + vp8_read(r, x->mb_segment_tree_probs[2]));
        else
            mi->segment_id = (unsigned char)(vp8_read(r, x->mb_segment_tree_probs[1]));
    }
	return;
}

void vp8_kfread_modes(VP8D_COMP *pbi)
{
    VP8_COMMON *const cp = & pbi->common;
    vp8_reader *const bc = & pbi->bc;

    MODE_INFO *m = cp->mi;
    const int ms = cp->mode_info_stride;

    int mb_row = -1;
    unsigned char prob_skip_false = 0;

    if (cp->mb_no_coeff_skip)
        prob_skip_false = (unsigned char)(vp8_read_literal(bc, 8));

    while (++mb_row < cp->mb_rows)
    {
        int mb_col = -1;

        while (++mb_col < cp->mb_cols)
        {
            MB_PREDICTION_MODE y_mode;

			//m->filter_level =255;

            // Read the Macroblock segmentation map if it is being updated explicitly this frame (reset to 0 above by default)
            // By default on a key frame reset all MBs to segment 0
            m->mbmi.segment_id = 0;

            if (pbi->mb.update_mb_segmentation_map)
                vp8_read_mb_features(bc, &m->mbmi, &pbi->mb);

            // Read the macroblock coeff skip flag if this feature is in use, else default to 0
            if (cp->mb_no_coeff_skip)
                m->mbmi.mb_skip_coeff = vp8_read(bc, prob_skip_false);
            else
                m->mbmi.mb_skip_coeff = 0;

            y_mode = (MB_PREDICTION_MODE) vp8_kfread_ymode(bc, cp->kf_ymode_prob);

            m->mbmi.ref_frame = INTRA_FRAME;

#ifdef FILTER_LEVEL
			if(pbi->mb.mode_ref_lf_delta_enabled)
				m->filter_level = pbi->mb.ref_lf_deltas[ m->mbmi.ref_frame]; //zou 1.18
#endif

            if ((m->mbmi.mode = y_mode) == B_PRED)
            {
                int i = 0;

                do
                {
                    const B_PREDICTION_MODE A = vp8_above_bmi(m, i, ms)->mode;
                    const B_PREDICTION_MODE L = vp8_left_bmi(m, i)->mode;
					m->bmi[i].mode = (B_PREDICTION_MODE) vp8_read_bmode(bc, cp->kf_bmode_prob [A] [L]);
                }
                while (++i < 16);
#ifdef FILTER_LEVEL
				m->filter_level  += pbi->mb.mode_lf_deltas[0]; //zou 1.18
#endif
            }
            else
            {
                int BMode;
                int i = 0;

                switch (y_mode)
                {
                case DC_PRED:
                    BMode = B_DC_PRED;
                    break;
                case V_PRED:
                    BMode = B_VE_PRED;
                    break;
                case H_PRED:
                    BMode = B_HE_PRED;
                    break;
                case TM_PRED:
                    BMode = B_TM_PRED;
                    break;
                default:
                    BMode = B_DC_PRED;
                    break;
                }
               do
                {
                    m->bmi[i].mode = (B_PREDICTION_MODE)BMode;
                }
                while (++i < 16);
            }

            (m++)->mbmi.uv_mode = (MB_PREDICTION_MODE)vp8_read_uv_mode(bc, cp->kf_uv_mode_prob);
        }

        m++; // skip the border
    }
	return;
}