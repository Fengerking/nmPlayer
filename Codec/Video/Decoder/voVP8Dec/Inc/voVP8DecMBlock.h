
/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __VP8_DEC_MBLOCK_H
#define __VP8_DEC_MBLOCK_H

#include "voVP8Decoder.h"

typedef unsigned char ENTROPY_CONTEXT;

typedef struct
{
    ENTROPY_CONTEXT y1[4];
    ENTROPY_CONTEXT u[2];
    ENTROPY_CONTEXT v[2];
    ENTROPY_CONTEXT y2;
} ENTROPY_CONTEXT_PLANES;  //zou 1230

#define Y1CONTEXT 0
#define UCONTEXT 1
#define VCONTEXT 2
#define Y2CONTEXT 3

#define MB_FEATURE_TREE_PROBS   3
#define MAX_MB_SEGMENTS         4

#define MAX_REF_LF_DELTAS       4
#define MAX_MODE_LF_DELTAS      4

// Segment Feature Masks
#define SEGMENT_DELTADATA   0
#define SEGMENT_ABSDATA     1

#define BLOCK_TYPES 4  //0 = Y no DC, 1 = Y2, 2 = UV, 3 = Y with DC */

#define prototype_subpixel_predict(sym) \
    void sym(unsigned char *src, int src_pitch, int xofst, int yofst, \
             unsigned char *dst, int dst_pitch)

typedef prototype_subpixel_predict((*vp8_subpix_fn_t));


typedef enum
{
    KEY_FRAME = 0,
    INTER_FRAME = 1
} FRAME_TYPE;

typedef struct
{
    short row;
    short col;
} MV;

typedef enum
{
    DC_PRED=0,            // average of above and left pixels
    V_PRED,             // vertical prediction
    H_PRED,             // horizontal prediction
    TM_PRED,            // Truemotion prediction
    B_PRED,             // block based prediction, each block has its own prediction mode

    NEARESTMV,
    NEARMV,
    ZEROMV,
    NEWMV,
    SPLITMV,

    MB_MODE_COUNT
} MB_PREDICTION_MODE;

// Macroblock level features
typedef enum
{
    MB_LVL_ALT_Q = 0,               // Use alternate Quantizer ....
    MB_LVL_ALT_LF = 1,              // Use alternate loop filter value...
    MB_LVL_MAX = 2,                 // Number of MB level features supported

} MB_LVL_FEATURES;


extern const int vp8_mb_feature_data_bits[MB_LVL_MAX] ;

// Segment Feature Masks
#define SEGMENT_ALTQ    0x01
#define SEGMENT_ALT_LF  0x02

#define VP8_YMODES  (B_PRED + 1)
#define VP8_UV_MODES (TM_PRED + 1)

#define VP8_MVREFS (1 + SPLITMV - NEARESTMV)

typedef enum
{
    B_DC_PRED=0,          // average of above and left pixels
    B_TM_PRED,

    B_VE_PRED,           // vertical prediction
    B_HE_PRED,           // horizontal prediction

    B_LD_PRED,
    B_RD_PRED,

    B_VR_PRED,
    B_VL_PRED,
    B_HD_PRED,
    B_HU_PRED,

    LEFT4X4,
    ABOVE4X4,
    ZERO4X4,
    NEW4X4,

    B_MODE_COUNT
} B_PREDICTION_MODE;

#define VP8_BINTRAMODES (B_HU_PRED + 1)  /* 10 */
#define VP8_SUBMVREFS (1 + NEW4X4 - LEFT4X4)

/* For keyframes, intra block modes are predicted by the (already decoded)
   modes for the Y blocks to the left and above us; for interframes, there
   is a single probability table. */

typedef struct
{
    B_PREDICTION_MODE mode;
    union
    {
        int as_int;
        MV  as_mv;
    } mv;
#ifdef MV_POSITION
	int mv_position;
#endif
} B_MODE_INFO;


typedef enum
{
    INTRA_FRAME = 0,
    LAST_FRAME = 1,
    GOLDEN_FRAME = 2,
    ALTREF_FRAME = 3,
    MAX_REF_FRAMES = 4
} MV_REFERENCE_FRAME;

typedef struct
{
    MB_PREDICTION_MODE mode, uv_mode;
    MV_REFERENCE_FRAME ref_frame;
    union
    {
        int as_int;
        MV  as_mv;
    } mv;
    char partitioning;
    //unsigned char partition_count;
    unsigned char mb_skip_coeff;                                //does this mb has coefficients at all, 1=no coefficients, 0=need decode tokens
    unsigned char dc_diff;
    unsigned char   segment_id;                  // Which set of segmentation parameters should be used for this MB
    
	 // Distance of MB away from frame edges
	int mb_to_left_edge;
    int mb_to_right_edge;
    int mb_to_top_edge;
    int mb_to_bottom_edge;
#ifdef MV_POSITION
	int mv_position;
#endif

} MB_MODE_INFO;


typedef struct
{
    MB_MODE_INFO mbmi;
	B_MODE_INFO bmi[16];
#ifdef FILTER_LEVEL
	int filter_level;     //zou 1.18
#endif
} MODE_INFO;

typedef struct
{
    short *qcoeff;
    //short *dqcoeff;
    unsigned char  *predictor;
    //short *diff;
    //short *reference;

    //short(*dequant)[4];

    // 16 Y blocks, 4 U blocks, 4 V blocks each with 16 entries
    unsigned char **base_pre;
    int pre;
    int pre_stride;

    unsigned char **base_dst;
    int dst;
    int dst_stride;

    //int eob;

    B_MODE_INFO bmi;

} BLOCKD;

typedef struct
{
	short qcoeff[400];
	unsigned char predictor[384];
	short* dq_y;
	short* dq_y2;
	short* dq_uv;

	char eob[25];
    BLOCKD block[25];

	int eobtotal;

    YV12_BUFFER_CONFIG pre; // Filtered copy of previous frame reconstruction
    YV12_BUFFER_CONFIG dst;

    MODE_INFO *mode_info_context;

    int mode_info_stride;

    FRAME_TYPE frame_type;

    int up_available;
    int left_available;

    // Y,U,V,Y2
	ENTROPY_CONTEXT_PLANES *above_context;
    ENTROPY_CONTEXT_PLANES *left_context;

	//int t[25];
    unsigned char segmentation_enabled; // 0 indicates segmentation at MB level is not enabled. Otherwise the individual bits indicate which features are active.
    unsigned char update_mb_segmentation_map; // 0 (do not update) 1 (update) the macroblock segmentation map.
    unsigned char update_mb_segmentation_data; // 0 (do not update) 1 (update) the macroblock segmentation feature data.
    unsigned char mb_segement_abs_delta; // 0 (do not update) 1 (update) the macroblock segmentation feature data.
    unsigned char mb_segment_tree_probs[MB_FEATURE_TREE_PROBS];         // Probability Tree used to code Segment number
    signed char segment_feature_data[MB_LVL_MAX][MAX_MB_SEGMENTS];            // Segment parameters

    // mode_based Loop filter adjustment
    unsigned char mode_ref_lf_delta_enabled;
    unsigned char mode_ref_lf_delta_update;

    signed char ref_lf_deltas[MAX_REF_LF_DELTAS];                     // 0 = Intra, Last, GF, ARF
    signed char mode_lf_deltas[MAX_MODE_LF_DELTAS];               // 0 = BPRED, ZERO_MV, MV, SPLIT

    vp8_subpix_fn_t  subpixel_predict;
    vp8_subpix_fn_t  subpixel_predict8x4;
    vp8_subpix_fn_t  subpixel_predict8x8;
    vp8_subpix_fn_t  subpixel_predict16x16;

    void *current_bc;

	long nCodecIdx;
	VO_CODEC_INIT_USERDATA * pUserData;
} MACROBLOCKD;


void vp8_build_block_doffsets(MACROBLOCKD *x);

#endif