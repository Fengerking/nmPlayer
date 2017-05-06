/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __VP8_DEC_COMMON_H
#define __VP8_DEC_COMMON_H

//#include "stdlib.h"
#include "voVP8DecBoolCoder.h"
#include "voVP8DecMBlock.h"
#include "voVP8LoopFilter.h"

#define MINQ 0
#define MAXQ 127
#define QINDEX_RANGE (MAXQ + 1)

#define NUM_YV12_BUFFERS 4

#define ALLOC_FAILURE -2

typedef struct frame_contexts
{
    unsigned char bmode_prob [VP8_BINTRAMODES-1];
    unsigned char ymode_prob [VP8_YMODES-1];   /* interframe intra mode probs */
    unsigned char uv_mode_prob [VP8_UV_MODES-1];
    unsigned char sub_mv_ref_prob [VP8_SUBMVREFS-1];
    unsigned char coef_probs [BLOCK_TYPES] [COEF_BANDS] [PREV_COEF_CONTEXTS] [vp8_coef_tokens-1];
    MV_CONTEXT mvc[2];
    MV_CONTEXT pre_mvc[2];  //not to caculate the mvcost for the frame if mvc doesn't change.
} FRAME_CONTEXT;

typedef enum
{
    ONE_PARTITION  = 0,
    TWO_PARTITION  = 1,
    FOUR_PARTITION = 2,
    EIGHT_PARTITION = 3
} TOKEN_PARTITION;

typedef enum
{
    RECON_CLAMP_REQUIRED        = 0,
    RECON_CLAMP_NOTREQUIRED     = 1
} CLAMP_TYPE;

typedef struct VP8Common
{
	short  Y1dequant[QINDEX_RANGE][16];  //zou 1230
	short  Y2dequant[QINDEX_RANGE][16];
	short  UVdequant[QINDEX_RANGE][16];

    int Width;
    int Height;

	int horiz_scale;
	int vert_scale;
#ifdef STABILITY
	int PreKF_Width;
	int PreKF_Height;
#endif

    YUV_TYPE clr_type;
    CLAMP_TYPE  clamp_type;

    YV12_BUFFER_CONFIG *frame_to_show;

    YV12_BUFFER_CONFIG yv12_fb[NUM_YV12_BUFFERS];
    int fb_idx_ref_cnt[NUM_YV12_BUFFERS];
    int new_fb_idx, lst_fb_idx, gld_fb_idx, alt_fb_idx;

    FRAME_TYPE last_frame_type;  //Add to check if vp8_frame_init_loop_filter() can be skipped.
    FRAME_TYPE frame_type;

    int show_frame;

    int frame_flags;
    int MBs;
    int mb_rows;
    int mb_cols;
    int mode_info_stride;

    // prfile settings
    int mb_no_coeff_skip;
    int no_lpf;
    int simpler_lpf;
    int use_bilinear_mc_filter;
    int full_pixel;
	 int (*build_uvmvs)(MACROBLOCKD *x,MODE_INFO *pModeInfo);
	

    int base_qindex;
    //int last_kf_gf_q;  // Q used on the last GF or KF

    int y1dc_delta_q;
    int y2dc_delta_q;
    int y2ac_delta_q;
    int uvdc_delta_q;
    int uvac_delta_q;

    MODE_INFO *mip; /* Base of allocated array */
    MODE_INFO *mi;  /* Corresponds to upper left visible macroblock */

    LOOPFILTERTYPE last_filter_type;
    LOOPFILTERTYPE filter_type;
    loop_filter_info lf_info[MAX_LOOP_FILTER+1];
    prototype_loopfilter_block((*lf_mbv));
    prototype_loopfilter_block((*lf_mbh));
	prototype_loopfilter_block((*lf_mbvh));
    prototype_loopfilter_block((*lf_bv));
    prototype_loopfilter_block((*lf_bh));
    int filter_level;
    int last_sharpness_level;
    int sharpness_level;

    int refresh_last_frame;       // Two state 0 = NO, 1 = YES
    int refresh_golden_frame;     // Two state 0 = NO, 1 = YES
    int refresh_alt_ref_frame;     // Two state 0 = NO, 1 = YES

    int copy_buffer_to_gf;         // 0 none, 1 Last to GF, 2 ARF to GF
    int copy_buffer_to_arf;        // 0 none, 1 Last to ARF, 2 GF to ARF

    int refresh_entropy_probs;    // Two state 0 = NO, 1 = YES

    int ref_frame_sign_bias[MAX_REF_FRAMES];    // Two state 0, 1

	ENTROPY_CONTEXT_PLANES *above_context;
	ENTROPY_CONTEXT_PLANES left_context;    

    // keyframe block modes are predicted by their above, left neighbors

    unsigned char kf_bmode_prob [VP8_BINTRAMODES] [VP8_BINTRAMODES] [VP8_BINTRAMODES-1];
    unsigned char kf_ymode_prob [VP8_YMODES-1];  /* keyframe "" */
    unsigned char kf_uv_mode_prob [VP8_UV_MODES-1];


    FRAME_CONTEXT lfc; // last frame entropy
    FRAME_CONTEXT fc;  // this frame entropy

    unsigned int current_video_frame;

    //int near_boffset[3];
    int version;

    TOKEN_PARTITION multi_token_partition;

	int baseline_filter_level[MAX_MB_SEGMENTS];

	long nCodecIdx;
	VO_CODEC_INIT_USERDATA * pUserData;
} VP8_COMMON;



void vp8_setup_version(VP8_COMMON *cm);
int vp8_alloc_frame_buffers(VP8_COMMON *oci, int width, int height);
void vp8_initialize_common();
void vp8_create_common(VP8_COMMON *oci);
void vp8_remove_common(VP8_COMMON *oci);
void vp8_yv12_extend_frame_borders(YV12_BUFFER_CONFIG *ybf);

#endif