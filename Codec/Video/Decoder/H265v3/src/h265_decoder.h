 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265_decoder.h
    \brief    h265 high level structure and function
    \author   Renjie Yu
	\change
*/


#ifndef __VOH265DECODER_H__
#define __VOH265DECODER_H__
#include "voH265.h"
#include "viMem.h"
#include "h265dec_bits.h"
#include "h265dec_porting.h"
//#include "h265dec_intra.h"
#include "h265dec_inter.h"
#include "h265dec_idct.h"
#include "h265dec_sao.h"
#include "h265dec_deblock.h"

//-----configuration
#define VO_MAX_INT                 2147483647  ///< max. value of signed 32-bit integer
#define MAX_LAYER_NUM              10
#define MAX_NUM_VPS                16

// AMVP: advanced motion vector prediction
#define AMVP_MAX_NUM_CANDS          2           ///< max number of final candidates
#define AMVP_MAX_NUM_CANDS_MEM      3           ///< max number of candidates

#define MAX_PPS_NUM                 64
#define MAX_SPS_NUM                 16

#define MIN_LOG_STORE_SIZE          2    //YU_TBD: should change to min_tb_size
#if 0

//add byDTT
#if defined(__GNUC__) && (__GNUC__ > 3 || __GNUC__ == 3 && __GNUC_MINOR__ > 0)
#define MAY_ALIAS __attribute__((may_alias))
#else
#define MAY_ALIAS
#endif
typedef union { VO_U16 i; VO_U8  c[2]; } MAY_ALIAS UNION16_TYPE;
typedef union { VO_U32 i; VO_U16 b[2]; VO_U8  c[4]; } MAY_ALIAS UNION32_TYPE;
typedef union { VO_U64 i; VO_U32 a[2]; VO_U16 b[4]; VO_U8 c[8]; } MAY_ALIAS UNION64_TYPE;
#define M16(src) (((UNION16_TYPE*)(src))->i)
#define M32(src) (((UNION32_TYPE*)(src))->i)
#define M64(src) (((UNION64_TYPE*)(src))->i)
#define CP16(dst,src) M16(dst) = M16(src)
#define CP32(dst,src) M32(dst) = M32(src)
#define CP64(dst,src) M64(dst) = M64(src)
//end of add
#endif


typedef struct {
	VO_CODEC_INIT_USERDATA *p_user_op;
	VO_MEM_VIDEO_OPERATOR *p_pic_buf_op;	/* outside memory operator*/
	VO_U32	codec_id;
}USER_OP_ALL;

typedef enum
{
	VIDEO_FRAME_I                = 0,   /*!< I frame */
	VIDEO_FRAME_P                = 1,   /*!< P frame */
	VIDEO_FRAME_B                = 2,   /*!< B frame */
	VIDEO_FRAME_S                = 3,   /*!< S frame */
	VIDEO_FRAME_NULL             = 4,   /*!< NULL frame*/
    VIDEO_FRAMETYPE_MAX			= VO_MAX_ENUM_VALUE
}
VIDEO_FRAMETYPE;

typedef enum 
{
    NAL_UNIT_CODED_SLICE_TRAIL_N = 0,   // 0
    NAL_UNIT_CODED_SLICE_TRAIL_R,   // 1
  
    NAL_UNIT_CODED_SLICE_TSA_N,     // 2
    NAL_UNIT_CODED_SLICE_TLA,       // 3   // Current name in the spec: TSA_R

    NAL_UNIT_CODED_SLICE_STSA_N,    // 4
    NAL_UNIT_CODED_SLICE_STSA_R,    // 5

    NAL_UNIT_CODED_SLICE_RADL_N,    // 6
    NAL_UNIT_CODED_SLICE_DLP,       // 7 // Current name in the spec: RADL_R

    NAL_UNIT_CODED_SLICE_RASL_N,    // 8
    NAL_UNIT_CODED_SLICE_TFD,       // 9 // Current name in the spec: RASL_R

    NAL_UNIT_RESERVED_10,
    NAL_UNIT_RESERVED_11,
    NAL_UNIT_RESERVED_12,
    NAL_UNIT_RESERVED_13,
    NAL_UNIT_RESERVED_14,
    NAL_UNIT_RESERVED_15,

    NAL_UNIT_CODED_SLICE_BLA,       // 16   // Current name in the spec: BLA_W_LP
    NAL_UNIT_CODED_SLICE_BLANT,     // 17   // Current name in the spec: BLA_W_DLP
    NAL_UNIT_CODED_SLICE_BLA_N_LP,  // 18
    NAL_UNIT_CODED_SLICE_IDR,       // 19  // Current name in the spec: IDR_W_DLP
    NAL_UNIT_CODED_SLICE_IDR_N_LP,  // 20
    NAL_UNIT_CODED_SLICE_CRA,       // 21
    NAL_UNIT_RESERVED_22,
    NAL_UNIT_RESERVED_23,

    NAL_UNIT_RESERVED_24,
    NAL_UNIT_RESERVED_25,
    NAL_UNIT_RESERVED_26,
    NAL_UNIT_RESERVED_27,
    NAL_UNIT_RESERVED_28,
    NAL_UNIT_RESERVED_29,
    NAL_UNIT_RESERVED_30,
    NAL_UNIT_RESERVED_31,

    NAL_UNIT_VPS,                   // 32
    NAL_UNIT_SPS,                   // 33
    NAL_UNIT_PPS,                   // 34
    NAL_UNIT_ACCESS_UNIT_DELIMITER, // 35
    NAL_UNIT_EOS,                   // 36
    NAL_UNIT_EOB,                   // 37
    NAL_UNIT_FILLER_DATA,           // 38
    NAL_UNIT_SEI,                   // 39 Prefix SEI
    NAL_UNIT_SEI_SUFFIX,            // 40 Suffix SEI
} NALU_TYPE ;

/// supported slice type
typedef enum
{
	B_SLICE,
	P_SLICE,
	I_SLICE
} SLICE_TYPE;


typedef struct
{
	VO_S16 m_iHor;     ///< horizontal component of motion vector
	VO_S16 m_iVer;     ///< vertical component of motion vector
}TComMv;


//JO++
/// supported prediction type
typedef enum 
{
  MODE_INTER,           ///< inter-prediction mode
  MODE_INTRA,           ///< intra-prediction mode
  MODE_SKIP,            ///< inter-skip mode 
  MODE_NONE = 0XFFFFFFFF
}PredMode;

/*Intra pred mode*/
#define INTRA_PLANAR             0
#define INTRA_DC                 1                     // index for intra DC mode
#define INTRA_ANGULAR26          26
#define INTRA_VER                26                    // index for intra VERTICAL   mode
#define INTRA_HOR                10                    // index for intra HORIZONTAL mode

#if REMOVE_LMCHROMA
#define NUM_CHROMA_MODE        5                     // total number of chroma modes
#else
#define NUM_CHROMA_MODE        6                     // total number of chroma modes
#endif
#define DM_CHROMA_IDX          36                    // chroma mode index for derived from luma intra mode


typedef enum
{
  PART_2Nx2N,           ///< symmetric motion partition,  2Nx2N
  PART_2NxN,            ///< symmetric motion partition,  2Nx N
  PART_Nx2N,            ///< symmetric motion partition,   Nx2N
  PART_NxN,             ///< symmetric motion partition,   Nx N
  PART_2NxnU,           ///< asymmetric motion partition, 2Nx( N/2) + 2Nx(3N/2)
  PART_2NxnD,           ///< asymmetric motion partition, 2Nx(3N/2) + 2Nx( N/2)
  PART_nLx2N,           ///< asymmetric motion partition, ( N/2)x2N + (3N/2)x2N
  PART_nRx2N,           ///< asymmetric motion partition, (3N/2)x2N + ( N/2)x2N
  PART_NONE = 0XFFFFFFFF
}PartMode;

typedef enum
{
  SCAN_DIAG  = 0,       ///< up-right diagonal scan
  SCAN_HOR,             ///< horizontal first scan
  SCAN_VER,             ///< vertical first scan
} COEFF_SCAN_TYPE;

typedef enum 
{
  TEXT_LUMA,            ///< luma
  TEXT_CHROMA,          ///< chroma (U+V)
  TEXT_CHROMA_U,        ///< chroma U
  TEXT_CHROMA_V,        ///< chroma V
  TEXT_ALL,             ///< Y+U+V
  TEXT_NONE = 15
}TextType;

/// reference list index
typedef enum
{
  REF_PIC_LIST_0 = 0,   ///< reference list 0
  REF_PIC_LIST_1 = 1,   ///< reference list 1
  REF_PIC_LIST_C = 2,   ///< combined reference list for uni-prediction in B-Slices
  REF_PIC_LIST_X = 100  ///< special mark
}RefPicList;

//Note: sizeof(PicMvField)==12, can't be changed!!!
typedef struct
{	
	VO_U8     pred_mode;
	VO_S8     m_iRefIdx[ 2 ];//YU_TBD: Fix it, bug!! should be VO_S16
	VO_U8     inter_dir;//YU_TBD: usage alignment
	VO_S32    m_acMv[ 2 ];
}PicMvField;


typedef struct{
	VO_S32  general_profile_space;
	VO_BOOL general_tier_flag;
	VO_S32  general_profile_idc;
	VO_BOOL general_profile_compatibility_flag[32];
	VO_S32  sub_layer_level_idc;
}ProfileTierLevel;

typedef struct{
  ProfileTierLevel m_generalPTL;
  ProfileTierLevel m_subLayerPTL[6];      // max. value of max_sub_layers_minus1 is 6
  VO_U32 sub_layer_profile_present_flag[6];
  VO_U32 sub_layer_level_present_flag[6];
}TComPTL;


typedef struct TComScalingList
{
    VO_S32      m_scalingListDC               [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM]; // lina, can delete ? !< the DC value of the matrix coefficient for 16x16
    VO_U32      scaling_list_pred_matrix_id                 [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM]; //!< RefMatrixID
    VO_S32      *ScalingList            [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM]; //!< quantization matrix    
} stScalingList;

typedef struct
{     
	//VO_U32 sps_video_parameter_set_id;  
	//VO_U32 sps_seq_parameter_set_id;  
	VO_U32 separate_colour_plane_flag;

	VO_U32 pic_width_in_luma_samples;   
	VO_U32 pic_height_in_luma_samples;    

	//contol para
    VO_U32 pic_width_in_ctbs_y;
	VO_U32 pic_height_in_ctbs_y;

	VO_U32 conf_win_left_offset;           
	VO_U32 conf_win_right_offset;          
	VO_U32 conf_win_top_offset;         
	VO_U32 conf_win_bottom_offset;     

	VO_U32 bit_depth_luma;
	VO_U32 bit_depth_chroma;

	VO_U32 log2_max_pic_order_cnt_lsb;
	VO_U32 sps_max_dec_pic_buffering_minus1[MAX_TLAYER];
	VO_U32 sps_max_num_reorder_pics[MAX_TLAYER];
	VO_U32 sps_max_latency_increase_plus1[MAX_TLAYER];

	VO_U32 log2_min_luma_coding_block_size;
	VO_U32 log2_luma_coding_block_size; 
	VO_U32 log2_min_transform_block_size;
	VO_U32 log2_max_transform_block_size; 
	VO_U32 max_transform_hierarchy_depth_inter;
	VO_U32 max_transform_hierarchy_depth_intra;
	VO_U32 scaling_list_enabled_flag;
	VO_U32 sps_scaling_list_data_present_flag;
	stScalingList spsScalingListEntity;
	VO_U32 amp_enabled_flag;
	VO_U32 sample_adaptive_offset_enabled_flag;
	VO_U32 pcm_enabled_flag;

	VO_U32 pcm_sample_bit_depth_luma_minus1;
	VO_U32 pcm_sample_bit_depth_chroma_minus1;
	VO_U32 log2_min_pcm_luma_coding_block_size;
	VO_U32 log2_max_pcm_luma_coding_block_size; 
	VO_U32 m_pcmLog2MaxSize;
	VO_U32 pcm_loop_filter_disable_flag;  

	VO_U32 num_short_term_ref_pic_sets;
	VO_U32 long_term_ref_pics_present_flag;
	VO_U32 num_long_term_ref_pic_sps;
	VO_U32 lt_ref_pic_poc_lsb_sps[ 33 ];//YU_TBD check num
	VO_U32 used_by_curr_pic_lt_sps_flag[ 33 ];
	
	VO_U32 sps_temporal_mvp_enable_flag;
	VO_U32 sps_strong_intra_smoothing_enable_flag;//YU_TBD, flag byte
	VO_U32 aspect_ratio;
	VO_BOOL Valid;    
    //VO_S32 *ExtendMinTbAddrZs; // malloc and free
	//VO_S32 *MinTbAddrZs; //YU_TBD: malloc and free
    //VO_U32 MinTbAddrZsStride;


#if 0
	VO_U32 log2_min_pcm_coding_block_size_minus3;
	VO_U32 log2_diff_max_min_pcm_coding_block_size;
	VO_U32 chroma_pred_from_luma_enabled_flag;       
	VO_U32 transform_skip_enabled_flag;             
	VO_U32 pps_loop_filter_across_slices_enabled_flag;           

	VO_U32 non_square_quadtree_enabled_flag;         
	VO_U32 sps_extension_data_flag;
//JO++
	//VO_S32 m_iAMPAcc[MAX_CU_DEPTH];
	//AMVP_MODE m_aeAMVPMode[MAX_CU_DEPTH];
	 // Max physical transform size	

        //-----Joyce add: for scalingList
        
#endif
} H265_DEC_SPS;

typedef struct
{
  VO_U32 m_uiTileWidth;
  VO_U32 m_uiTileHeight;
  VO_U32 m_uiRightEdgePosInCU;
  VO_U32 m_uiBottomEdgePosInCU;
  VO_U32 m_uiFirstCUAddr;
} H265_DEC_TILE;


typedef struct {
	// Explicit weighted prediction parameters parsed in slice header,
	// or Implicit weighted prediction parameters (8 bits depth values).
	VO_U32         weight_flag;
	VO_U32         log2_weight_denom;
	VO_S32         weight;
	VO_S32         offset;

	// Weighted prediction scaling values built from above parameters (bitdepth scaled):
	//VO_S32         w, o, offset, shift, round;
} wpScalingParam;


typedef struct
{
	//VO_U32 pps_pic_parameter_set_id;
	VO_U32 pps_seq_parameter_set_id;
	VO_U32 dependent_slice_segments_enabled_flag;
	VO_U32 output_flag_present_flag;
    VO_S32 num_extra_slice_header_bits;
	VO_U32 sign_data_hiding_enabled_flag;
	VO_U32 cabac_init_present_flag;
	VO_U32 num_ref_idx_l0_default_active;
	VO_U32 num_ref_idx_l1_default_active;
	VO_S32 pic_init_qp_minus26;
	VO_U32 constrained_intra_pred_flag;
	VO_U32 transform_skip_enabled_flag;               

	VO_U32 cu_qp_delta_enabled_flag;
	VO_U32 diff_cu_qp_delta_depth;
	VO_S32 pps_chroma_qp_offset[ 2 ];
	//VO_S32 pps_cr_qp_offset;
	VO_U32 pps_slice_chroma_qp_offsets_present_flag;
	VO_U32 weighted_pred_flag;
	VO_U32 weighted_bipred_flag;
	VO_U32 transquant_bypass_enabled_flag;
	VO_U32 tiles_enabled_flag;
	VO_U32 entropy_coding_sync_enabled_flag;
	VO_U32 num_tile_columns_minus1;
	VO_U32 num_tile_rows_minus1;
	VO_U32 uniform_spacing_flag;
	VO_U32* column_width;
	VO_U32* row_height;
	VO_U32 loop_filter_across_tiles_enabled_flag;
	VO_U32 pps_loop_filter_across_slices_enabled_flag;
	VO_U32 deblocking_filter_control_present_flag;
	VO_U32 deblocking_filter_override_enabled_flag;
	VO_U32 pps_deblocking_filter_disabled_flag;
	VO_S32 pps_beta_offset_div2;
	VO_S32 pps_tc_offset_div2;
	VO_U32 pps_scaling_list_data_present_flag;
    VO_U32 lists_modification_present_flag;
	stScalingList ppsScalingListEntity;
	VO_U32 log2_parallel_merge_level_minus2;
	VO_U32 slice_segment_header_extension_present_flag;


	VO_U32 m_iNumSubstreams;//YU_TBD
    VO_U32 *CtbAddrRsToTs;//YU_TBD :malloc and free
	VO_U32 *CtbAddrTsToRs;//YU_TBD :malloc and free
	VO_U32 *TileId;//YU_TBD:malloc and free
    VO_S32 *ExtendMinTbAddrZs; // malloc and free
	VO_S32 *MinTbAddrZs; //YU_TBD: malloc and free
    VO_U32 MinTbAddrZsStride;

	VO_BOOL Valid;
    VO_S32 tile_start_zs[MAX_ENTRY_SIZE];//YU_TBD: check the usage

} H265_DEC_PPS;


typedef struct
{
	//rps
	//VO_U32 delta_idx_minus1;
	//VO_U32 delta_rps_sign;
	//VO_U32 abs_delta_rps_minus1;
	VO_U32 used_by_curr_pic_flag;
	VO_U32 use_delta_flag;
	VO_U32 num_negative_pics;
	VO_U32 num_positive_pics;
	//VO_U32 delta_poc_s0_minus1;
	//VO_U32 used_by_curr_pic_s0_flag;
	VO_U32 delta_poc_s1_minus1;
	//VO_U32 used_by_curr_pic_s1_flag;
	VO_U32 m_numberOfPictures;//setNumberOfPictures;
	VO_S32 delta_poc[MAX_NUM_REF_PICS];
	VO_U32 used[MAX_NUM_REF_PICS];
	VO_U32 num_long_term_pics;//");     rps->setNumberOfLongtermPictures(uiCode);
	//VO_U32 m_numberOfPictures;
	VO_S32 m_POC[MAX_NUM_REF_PICS];
	VO_BOOL m_bCheckLTMSB[MAX_NUM_REF_PICS];
} H265_DEC_RPS;

typedef struct TUTask
{
  VO_U8 intra_flag;
  VO_U8 trans_flag;
  //param for intra
  VO_U16 x0;
  VO_U16 y0;
  VO_U8 log2TrafoSize;
  VO_U8 IntraPredMode;
  VO_U8 cIdx;
  VO_U32 available_flag;
  //param for trans
  VO_U8 function_index;
  VO_U8 width;
  VO_U16 pred_stride;
  VO_S16* TransCoeffLevel;
  VO_U8* p_pred_buf;
  //VO_U32 cu_end;
}TUTask;

typedef struct PUTask
{
  //VO_U32 unvaild;
  VO_U16 x0;
  VO_U16 y0;
  VO_U8 nPbW; 
  VO_U8 nPbH;
  PicMvField pb_mvfield;
  //VO_U32 cu_end;
}PUTask;

struct H265_DEC_SLICE;
typedef struct TReconTask
{
  struct H265_DEC_SLICE *p_slice;
#if USE_3D_WAVE_THREAD
  h265_wave3DThdInfo *pThdInfo;
#endif

  VO_U32 ctb_addr_in_rs;
  VO_U32 nRow;
  VO_U32 nColumn;

  volatile VO_U32 bFinish;
  struct TReconTask *blocked;
  struct TReconTask *next_blocked;

  VO_U16 start_pu;
  VO_U16 total_pu;
  VO_U16 start_tu;
  VO_U16 total_tu;
  TUTask tu_task[384];
  PUTask pu_task[256];
}TReconTask;

//-----------pic
typedef struct TComPic
{
  VIDEO_FRAMETYPE pic_type;
  VO_U8*  pic_buf[ 3 ];           ///< Buffer (including margin)
  VO_S32  pic_stride[ 3 ];           ///< Buffer (including margin)
//   VO_S16* coeff_buf[3];
//   VO_S16* p_coeff[3];
  VO_S32  m_iPicWidth;            ///< Width of picture
  VO_S32  m_iPicHeight;           ///< Height of picture
  VO_S32  m_iPOC;

  VO_S32 m_disWidth;
  VO_S32 m_disHeight;

  VO_BOOL   m_bIsLongTerm;            //  IS long term picture
  VO_BOOL	m_bRefenced;
#if USE_CODEC_NEW_API
#define PIC_READY_FOR_OUT		2
#define PIC_OUT		3
#endif
  VO_S32	m_bNeededForOutput;      // True: ready for output. 

//   TReconTask *rc_task;
  struct H265_DEC_SLICE *p_slice;

  PicMvField *motion_buffer; 
  VO_S32      m_aiRefPOCList[ 2 ][ MAX_NUM_REF_PICS + 1 ]; //m_aiRefPOCList in silce is pointed to this.


#if REMOVE_APS
 // TComSampleAdaptiveOffset *m_saoParam;
#endif
#if USE_3D_WAVE_THREAD
// 	volatile VO_S32 nReady_y;
//   volatile VO_S32 nProgress;
  volatile VO_S32 nProgress_mv;
  volatile VO_S32 errFlag;
  volatile VO_U32 bFinishDecode;
  pthread_mutex_t unBlockMutex;
#endif
	VO_S64 Time;
} TComPic;

#define MAX_NUM_CTX_MOD             186       ///< maximum number of supported contexts

typedef struct {
  
  VO_S32 low_bits;
  VO_S32 range;
  const VO_U8 *stream_start;
  const VO_U8 *stream;
  const VO_U8 *stream_end;
  BIT_STREAM bs;
    // Context Model
  VO_U8   contextModels[MAX_NUM_CTX_MOD];
	
} H265_DEC_CABAC;

//////DPB management


typedef struct H265_LIST_NODE
{
	TComPic* data;
// 	struct H265_LIST_NODE* previous;
// 	struct H265_LIST_NODE* next;

	//Harry: just save the offset from pool, for easy backup
	VO_U32 previous;
	VO_U32 next;
}H265_LIST_NODE;


//Max dispaly pic buffer 60
#define MAX_DPB_SIZE      (MAX_NUM_REF_PICS + 1 + 60)      
typedef struct H265_DPB_LIST
{
	H265_LIST_NODE* head;
	H265_LIST_NODE* real_head;
	H265_LIST_NODE* tail;
	VO_U32 size;
	VO_U32 real_size;
	H265_LIST_NODE* p_fifo;
	VO_U32 node_num;
	H265_LIST_NODE list_node_pool[MAX_DPB_SIZE];

	//For display management
	H265_LIST_NODE* dis_head;
	H265_LIST_NODE* dis_tail;
	H265_LIST_NODE dis_node_pool[MAX_DPB_SIZE];//YU_TBD need to check the max reorder number
	VO_S32          m_iPOCLastDisplay;
	volatile VO_S32			disListLeft;	//count the free dis_list room left num
}H265_DPB_LIST;

typedef struct H265_ENTRY{	
//     VO_S32 entryPointOffsets;
//     H265_DEC_CABAC *p_cabac;

    //VO_S32 last_coded_qp;
    //VO_S32 cu_qp;

    //VO_U32  m_bDecodeDQP;

    //VO_S32	 m_codedQP;
    //VO_S32	 dequant_qp[4];	

    //VO_S32 tile_start_zs;
    VO_S32 entry_point_offset;
    VO_S32 dequant_qp[3];  //cidx

}H265_ENTRY;

/////////////////
typedef struct H265_DEC_SLICE
{
	VO_S32 priv_mem_cfg;
	VO_U8*  p_priv_mem;

	H265_DEC_SPS *p_sps;
	H265_DEC_PPS *p_pps;
	H265_DEC_RPS *p_rps;
	H265_DEC_RPS local_rps;
	VO_U32 slice_seq_parameter_set_id; //Harry add.  Cur Pic SPS ID
	VO_U32 slice_pic_parameter_set_id;

	//slice header
	VO_U32 first_slice_segment_in_pic_flag;//YU_TBD; byte for flag
    VO_U32 dependent_slice_segment_flag;
	VO_U32 slice_segment_address;
	VO_U32 slice_end_address;
	VO_U32 SliceAddrRs;
	VO_S32 SliceMinTbAddrZs;
    VO_U32 slice_type;  
	VO_U32 enable_temporal_mvp_flag;
	VO_U32 slice_sao_luma_flag;
	VO_U32 slice_sao_chroma_flag;
	VO_U32 num_ref_idx[3]; 
	//piclist
	VO_U32  ref_pic_list_modification_flag_l0;
	VO_U8   list_entry_l0[32];
	VO_U32  ref_pic_list_modification_flag_l1;
	VO_U32  list_entry_l1[32];
	VO_U32  mvd_l1_zero_flag;
	VO_U32 	cabac_init_flag;
	VO_U32  collocated_from_l0_flag;
	VO_U32  collocated_ref_idx;
	//xParsePredWeightTable
	VO_U32  max_num_merge_cand;
	VO_S32 slice_qp;
	VO_S32 slice_qp_delta_cb;
	VO_S32 slice_qp_delta_cr;
	VO_U32 slice_deblocking_filter_disabled_flag ;
	VO_U32 beta_offset_div2;
	VO_U32 tc_offset_div2;
	VO_U32 slice_loop_filter_across_slices_enabled_flag;
	VO_U32 num_entry_point_offsets;
    //VO_U32* entry_point_offset;
//   VO_U32 emulation_prevention_byte_offset[MAX_EMULATE_SIZE]; //indicate position of each emulation_prevention_byte
//   VO_U32 emulation_prevention_byte_count; //indicate number of emulation_prevention_byte in this NALU

	//Local value
	VO_U32  nal_unit_type;
    VO_U32  m_uiTLayer;
	VO_S32	m_iPOC;
	VO_BOOL  m_bCheckLDC;

	TComPic*    ref_pic_list [ 2 ][ MAX_NUM_REF_PICS + 1 ];
	VO_S32      (*ref_poc_list)[ 2 ][ MAX_NUM_REF_PICS + 1 ];
	TComPic*	cur_pic;

	//--------properties values
    VO_U32 MinCbLog2SizeY;
	VO_U32 MinTbLog2SizeY;
	VO_U32 CtbLog2SizeYMask;
    VO_U32 CtbLog2SizeY;
    VO_U32 PicWidthInCtbsY;
	VO_U32 PicHeightInCtbsY;
	VO_U32 PicWidthInMinCbsY;
	VO_U32 PicHeightInMinCbsY;
	VO_U32 PicWidthInMinTbsY;
	VO_U32 PicHeightInMinTbsY;
    VO_U32 PicSizeInCtbsY;
	VO_U32 log2_diff_max_min_luma;
	VO_U32 log2_diff_max_min_luma_mask;
	//VO_U32 log2_diff_max_min_luma_pu;
	VO_U32 log2_diff_max_min_luma_pu_mask;
	VO_U32 partition_num;//YU_TBD:remove it
	//------------------------------------------------

  H265_DEC_CABAC *pCabac;
	H265_DEC_CABAC cabac[MAX_ENTRY_SIZE];//YU_TBD move to slice
	BIT_STREAM bs;

#define CU_SPLIT_FLAG 0x01
#define CU_SKIP_FLAG  0x02
#define CU_PRED_MODE  0x04

	VO_S32 cur_slice_number; //add by DTT
	VO_U8 *left_depth_buffer;
	VO_U8 *top_depth_buffer;
	VO_U8 *left_skip_buffer;
	VO_U8 *top_skip_buffer;
	VO_S8 *pu_para_top;
	VO_S8 *pu_para_left;
    //VO_S8 *slice_number;  removed by DTT
	VO_U32 *deblock_para; //pred_mode(1byte) | qp(1byte) | cbf_luma (1byte) | pcm_flag(1byte) 

	//CTU
// 	VO_U32 tile_start_x;//add by DTT
// 	VO_U32 tile_start_y;//add by DTT  
// 	VO_S32 tile_end_x;//add by DTT
// 	VO_S32 tile_end_y;//add by DTT
    //VO_S32 tile_start_zs;

	VO_U32 split_cu_flag;//YU_TBD:malloc or size
	VO_U32 IsCuQpDeltaCoded;
    TReconTask* cur_rctask;    
	//CU
	VO_U32 pcm_flag;
	VO_U32 depth;
	VO_U32 cu_transquant_bypass_flag;
	VO_U32 cu_skip_flag;
	PredMode CuPredMode;//YU_TBD: Store for intra mode prediction
	PartMode CuPartMode;

	//PU
	VO_U32 IntraSplitFlag;//YU_TBD: check store
	VO_U8  IntraPredModeY[ 4 ];//YU_TBD: check store
	VO_U32 IntraPredModeC;//YU_TBD: check store
	VO_U32 IntraPredMode_Y;
	VO_U32 merge_flag;//YU_TBD: check store
	PicMvField* p_motion_field;

	//Deblocking elements
	VO_U8   *p_pBsHor;
	VO_U8   *p_pBsVer;
	///////////////////////////
	VO_U8*  edge_emu_buff0;
	VO_U8*  edge_emu_buff1;

    VO_S16* asm_mc0_buff;
    VO_S16* asm_mc1_buff;

	//VO_U32 inter_pred_idc;//YU_TBD:malloc
	//VO_U8  ref_idx_l0;//YU_TBD:malloc
	//VO_U8  ref_idx_l1;//YU_TBD:malloc


	//TU
	VO_U32 MaxTrafoDepth;	

	
	//YU_TBD: One-dimensional
	VO_U8 cbf_cb[ 5 ][ 256 ];//bit :[0~4] trafoDepth, //YU_TBD:check the size according to min transform size??or 128?? and depth
	VO_U8 cbf_cr[ 5 ][ 256 ];//bit :[0~4] trafoDepth, //YU_TBD:check the size according to min transform size??
	VO_U32 cbf_luma;//bit :[0~4] trafoDepth, //YU_TBD:check the size according to min transform size??

	//Residual buffer

	//VO_S16 *TransCoeffLevel;
	//VO_S32 QpY;
    //VO_S32 dequant_qp[3];  //cidx
	TComSampleAdaptiveOffset **m_saoParam;//SAO
	//get available
	/////////////////////////////////////////////////////////////////////////////
//	VO_BOOL *m_bCULeft;
	//VO_BOOL *m_bCUAbove;
	//VO_BOOL *m_bCUAboveRight;
//	VO_BOOL *m_bCUAboveLeft;
	//VO_U32 *m_puiTileIdxMap;

	VO_U32 m_uiCUAddr;
	//VO_U32 m_uiMinCUWidth      ;
	//VO_U32 m_uiMinCUHeight     ;
//	VO_U32 m_uiNumPartInWidth  ;
	//VO_U32 m_uiNumPartInHeight ;
	//VO_U32 partition_num;
	//VO_U32 m_uiWidthInCU       ;
	//VO_U32 m_uiHeightInCU      ;
	//COM_DATA_CU*   m_apcCUColocated[2];  ///< pointer of temporally colocated CU's for both directions
	////////////////////////////////////////////////////////////////////////////////////////////////

	//VO_U8   *m_puhDepth;
	//VO_U8   *m_puhWidth;         
	//VO_U8   *m_puhHeight;        

	//VO_U8   *m_puBsBuffer[2]; //ZOU for deblock
#if USE_3D_WAVE_THREAD
	VO_U8   **m_puBufForIntraY; //Harry bak for intra predict
	VO_U8   **m_puBufForIntraU; //Harry bak for intra predict
	VO_U8   **m_puBufForIntraV; //Harry bak for intra predict
#else
	VO_U8   *m_puBufForIntraY[2]; //ZOU bak for intra predict
	VO_U8   *m_puBufForIntraU[2]; //ZOU bak for intra predict
	VO_U8   *m_puBufForIntraV[2]; //ZOU bak for intra predict
#endif

	//JO++
	//VO_S32   m_codedQP;
	//VO_U8   m_puhTransformSkip[3][256];///< array of transform skipping flags
	//VO_S8   m_apiMVPIdx[2][256]; 
	//VO_S8   m_apiMVPNum[2][256]; 
	//VO_BOOL m_abEqualRef  [2][MAX_NUM_REF_PICS][MAX_NUM_REF_PICS];
	//VO_S16  m_pcIPCMSampleY[256];      ///< PCM sample buffer (Y)
    //VO_S16  m_pcIPCMSampleCb[256];     ///< PCM sample buffer (Cb)
   // VO_S16  m_pcIPCMSampleCr[256];     ///< PCM sample buffer (Cr)

	//VO_BOOL m_bNextDependentSlice;
	//VO_BOOL m_bNextSlice;
	//VO_BOOL m_bCheckLDC;



	//VO_U32  m_uhTotalDepth;

	//TComCUMvField* p_mv_field;
   // PicMvField* p_motion_field;
	//VO_U8   *p_depth;
	//VO_S8   *p_qp;
	//VO_U8   *p_predmode;
	//VO_U8   *p_predmode_tmp; //add by DTT
	//VO_U8   *p_luma_intra_dir;
	//VO_U8	*p_width;
	//VO_U8	*p_height;
	//VO_S8   *qp_buffer;
	//JO--

   // VO_U8   *skip_buffer; //add by DTT
   // VO_S8  *slice_number;  //add by DTT
   // VO_U32 cur_slice_number; //add by DTT
   // VO_S32 tile_start_x;//add by DTT
	//VO_S32 tile_start_y;//add by DTT
	//VO_U8  *left_depth_buffer;//add by DTT
	//VO_U8  *top_depth_buffer;//add by DTT
	//VO_U8 *left_luma_intra_mode_buffer;//add by DTT
	//VO_U8 *top_luma_intra_mode_buffer;//add by DTT
	//VO_U8 *inter_dir_buffer;//add by DTT
	//VO_U8 *luma_cbf;
	//VO_U8* p_tr_idx;
   // VO_U8 *m_puhLumaIntraDir;
   // VO_U8 m_puhChromaIntraDir[256];
	//VO_U8 *m_puhTrIdx;

	//VO_U32  m_uiCUPelX;
	//VO_U32  m_uiCUPelY;
	//VO_U32  m_uiAbsIdxInLCU; 
	//VO_U32  m_uiAbsIdxInLCU_recon;
	//VO_U32  m_bDecodeDQP;
    
    //VO_S16       m_pcTrCoeffY[256*16];         ///< transformed coefficient buffer (Y)
   // VO_S16       m_pcTrCoeffCb[256*16];        ///< transformed coefficient buffer (Cb)
   // VO_S16       m_pcTrCoeffCr[256*16];        ///< transformed coefficient buffer (Cr)
	//JO--
    // add(Joyce) for special dequant and idct
   // VO_U8       m_pcTrCoeffY_NZ_block[256];         ///< transformed coefficient buffer (Y)
    //VO_U8       m_pcTrCoeffCb_NZ_block[256];        ///< transformed coefficient buffer (Cb)
    //VO_U8       m_pcTrCoeffCr_NZ_block[256];        ///< transformed coefficient buffer (Cr)

    //=============== Recon data structure definitions here!!! =================
    //INTRA_PRED_PARAM m_intraPred;              /// prediction.
    //INVERSE_TRANSFORM_BASE_INFO m_invTransform; /// idct
    //STRUCT_FOR_DEQUANT m_dequant;                       /// dequant   
    //TComPic m_interCache;                      /// mc cache.

    //------------------Joyce add: scalingList-----------
    stScalingList * pSliceScalingListEntity;
    wpScalingParam  m_weightPredTable[2][MAX_NUM_REF_PICS][3]; // [REF_PIC_LIST_0 or REF_PIC_LIST_1][refIdx][0:Y, 1:U, 2:V]
    // add for global array.
    VO_U32* g_auiSigLastScan[3][ MAX_CU_DEPTH ];
    VO_S16 * MMulLevelScale[SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM][SCALING_LIST_REM_NUM]; //8.6.3,  m array * level scale

	//add for wpp
	//H265_DEC_CABAC TR_cabac_bac;
	//VO_U32 m_uilastsubstream;
	//const VO_U8  *p_bit_start;

	//Deblocking elements
	//VO_U32  m_xAddr;
	//VO_U32  m_yAddr;
	//VO_U8   *p_pBs[2];
	
	//add for pu by DTT
	//VO_U8 luma_intra_mode[4];
	//VO_U32 transquant_bypass_cu;
	//VO_U32 skip_cu;
	//VO_U32 part_mode;
	//VO_U32 part_size;
	//VO_U32 pcm_cu;
	//VO_U32 merge_pu;
	//VO_S32 intra_split;
	//VO_U8 cbf_y[5][256];
	//VO_U8 cbf_u[5][256];
	//VO_U8 cbf_v[5][256];
#if USE_3D_WAVE_THREAD
	VO_S32 *filterX_Pos;
#else
	VO_S32 filterX_Pos;
#endif
	VO_BOOL init_done;			//Harry add
	VO_S32 nThdNum;				//Harry add
	const USER_OP_ALL * p_user_op_all; //Harry add
	VO_U32 fast_mode;			//Harry add
	volatile VO_S32 errFlag;	//Harry add
#if USE_3D_WAVE_THREAD
	H265_DPB_LIST dpb_list_s;
	h265_wave3DThdInfo *pThdInfo;
	VO_U8 *pFrameData;			//save the frame data to be decoded
	VO_U32 frameDataSize;		//data length is frameDataSize*INPUT_SIZE_INC_STEP

	VO_U8* in_data;				//the stream pos for DoDecode after Prepare
	VO_S32 data_sz;				//the left length for DoDecode after Prepare
#endif
	VO_S16* coeff_buf[3];
	VO_S16* p_coeff[3];
	TReconTask *rc_task;

  //add for pu by DTT
  VO_S32 currEntryNum;
  H265_ENTRY entries[MAX_ENTRY_SIZE];
  VO_U32 emulation_prevention_byte_count; //indicate number of emulation_prevention_byte in this NALU
  VO_U32 emulation_prevention_byte_offset[MAX_EMULATE_SIZE]; //indicate position of each emulation_prevention_byte
  
  // move from sao
  VO_U32          *m_sliceIDMapLCU; //slice ID of each LCU
  NDBFBlockInfo *m_vNDFBlock;
  VO_BOOL  m_bUseNIF;       //!< true for performing non-cross slice boundary ALF   
  SaoLcuParam* saoLcuParam;
  SaoOffsetTable *pSaoOffsetTable;   // one more than actual size, ,because mergeLeftFlag
  VO_U8   *m_lumaTableBo;
	//VO_U32  uiFrameStride4bk;
  VO_U8 *voCabacInitTab;
  VO_S32 dbk_flag;
  VO_S32 sao_flag;
	VO_S64 timestamp;
} H265_DEC_SLICE;


typedef struct
{
	VO_U32 sps_video_parameter_set_id;
	VO_U32 vps_max_layers_minus1;
	VO_U32 vps_temporal_id_nesting_flag;
	//TComPTL  m_pcPTL;
	VO_U32 vps_max_dec_pic_buffering_minus1[MAX_TLAYER];
    VO_U32 vps_num_reorder_pics[MAX_TLAYER];
    VO_U32 vps_max_latency_increase_plus1[MAX_TLAYER];
	VO_U32 vps_max_op_sets_minus1;
#if VPS_OPERATING_POINT
	VO_U32 vps_num_layer_sets_minus1;
	VO_U32 vps_max_layer_id;
	VO_BOOL layer_id_included_flag[MAX_VPS_NUM_HRD_PARAMETERS_ALLOWED_PLUS1][MAX_VPS_NUH_RESERVED_ZERO_LAYER_ID_PLUS1];
#endif
  VO_BOOL Valid;

} H265_DEC_VPS;

#if 0
#if SIGNAL_BITRATE_PICRATE_IN_VPS
typedef struct{
  VO_BOOL        m_bitRateInfoPresentFlag[MAX_TLAYER];
  VO_BOOL        m_picRateInfoPresentFlag[MAX_TLAYER];
  VO_S32         m_avgBitRate[MAX_TLAYER];
  VO_S32         m_maxBitRate[MAX_TLAYER];
  VO_S32         m_constantPicRateIdc[MAX_TLAYER];
  VO_S32         m_avgPicRate[MAX_TLAYER];
}TComBitRatePicRateInfo;
#endif
#endif
//////////////////////////////////
typedef struct H265_DEC
{
    VO_U32 pic_width_in_luma_samples;// keep as global pic size, 
	VO_U32 pic_height_in_luma_samples;// 
	
	//Harry: add for BA max size
	VO_U32 using_BA_size;
	VO_U32 BA_max_width;
	VO_U32 BA_max_height;

	//@Harry: all thread will use them
	H265_DEC_VPS vps[ 16 ];//YU_TBD:Check num
	H265_DEC_SPS sps[ MAX_SPS_NUM ];//YU_TBD:Check num
	H265_DEC_PPS pps[ MAX_PPS_NUM ];//YU_TBD:Check num
	H265_DEC_RPS rps[ 65 ];
	//global value
	VO_S32 m_prevPOC;

	H265_DEC_SLICE slice;//YU_TBD multi
	//H265_DEC_CABAC cabac;//YU_TBD move to slice

	//Control
	BIT_STREAM bit_stream;
	USER_OP_ALL user_op_all;

	VO_S32	frame_num;
	VO_S32	out_mode;	
	VO_S32  input_mode;
	VO_S32  flush_dis_pics;//if 1, need to flush all decoded pics
	VO_S32  fast_mode;	//Harry: used for enable fast mode: 0:normal, 1:fast, 2:very fast, 3:utrafast(drop frame)
	VO_S32  thumbnail_mode;//VO_PID_VIDEO_THUMBNAIL_MODE = 1;

	//dpb
	VO_U8*         dpb_buf;
 	TComPic        dpb_pool[ MAX_DPB_SIZE ];
 	H265_DPB_LIST  dpb_list;



	//TComPic *dpb_pool;
	//H265_DPB_LIST *dpb_list;
	VO_U32 pic_buf_done; //Harry add. to make sure dpb init done only once

	//nal header
	//VO_S32 nal_unit_type;
    //Slice
    //VO_S32 *MinCbAddrZS;
	//VO_S32 pic_parameter_set_id;
	//VO_BOOL *m_bCULeft;
	//VO_BOOL *m_bCUAbove;
	//VO_BOOL *m_bCUAboveRight;
	//VO_BOOL *m_bCUAboveLeft;

	//VO_S32 pic_nal_len;
	//VO_U8* nal_start_pos;

	//Tile
	//H265_DEC_TILE *m_apcTComTile;
	//TComBitRatePicRateInfo    m_bitRatePicRateInfo;
	
  //VO_BOOL m_already_print_stream_info;
  //VO_U32 start_time;
  //VO_U32 params_init_done;
  //multi-core
  VO_S32 nThdNum;
  volatile VO_U32 bOutputFull;
#if USE_3D_WAVE_THREAD
  h265_wave3DThdPool *pWave3DThdPool;
  VO_S32 nDecodingFrame;
  H265_DEC_SLICE *slicePool;
  VO_U8* in_data;
  VO_U32 data_sz;
  VO_U32 frameDataSize;		//data length is frameDataSize*INPUT_SIZE_INC_STEP

//   TaskQ freeFrameList;
//   TaskQ parserTaskQ;
//   TaskQ reconTaskQ;
#endif
#if USE_CODEC_NEW_API
  pthread_mutex_t disListMutex;
  pthread_cond_t disListCond;
  pthread_cond_t *getPicCond;
#endif
  VO_S64 timestamp;
  VO_U8 *voCabacInitTab;
} H265_DEC;


////////////////inter memory
/* */
#define INNER_MEM_DATA_PARTITION	1
#define INNER_MEM_FRAME_MEME		2
#define INNER_MEM_FRONT_HANDLE		4
#define INNER_MEM_DEC_HANDLE		8

#define INNER_MEM_MAX		(INNER_MEM_DATA_PARTITION | INNER_MEM_FRAME_MEME |\
							INNER_MEM_FRONT_HANDLE | INNER_MEM_DEC_HANDLE)

typedef struct 
{
	VO_S32 pic_width;				/*!< picture pic_width offered outside*/
	VO_S32 pic_height;				/*!< picture pic_height offered outside*/
	VO_S32 expic_width;				/*!< including padding */
	VO_S32 expic_height;			/*!< including padding */
	VO_S32 pic_num;					/*!< number of reference and reconstruction  frames */
	VO_S32 private_mem_size;		/*!< size in byte */
	VO_U32 resolved;
	VO_U8 *pic_buf[MAX_DPB_SIZE][3];
	VO_U8 *p_priv_mem;
}VIDEO_INNER_MEM;



#define RAW_DATA_LEN 1024*1024*20

#define PIC_MODE                 0
#define STREAM_MODE              1

typedef struct{	
	VO_U32 codec_id;
	//VIDEO_INNER_MEM       inner_mem;
	VO_CODEC_INIT_USERDATA  *p_user_op;
	//VO_CODECBUFFER          codec_buf;
	VO_U32					input_mode;//0: pic mode 1:stream mode
	VO_U32                  input_len;
	///////////internal rawdata buffer control
	VO_U8*                  p_input_buf;//Internal rawdata buffer
    VO_U8*					p_cur_buf_pos;//Current rawdata buffer position
    VO_U32					left_buf_len;//Unused rawdata buffer
	VO_U32			        used_buf_len;
	VO_S32                  more_dis_pics;//remain pics to be displayed, including currrent dis pic which buf is not ready
	//Harry add
	volatile VO_U32               new_input;
	const VO_U8 *				lastPos;
	VO_U32				bFindFirstSlice;
	VO_U32				bNeedFlush;
	VO_U32				frameLen[MAX_FRAME_INPUT];
#if ENABLE_DROP_FRAME
	VO_U32				bCanDrop[MAX_FRAME_INPUT+1];
	VO_U32				nContinueDropped;	//count the num of continually dropped frame
#endif
	VO_S64				Time[MAX_FRAME_INPUT+1];
#if ENABLE_LICENSE_CHECK
	VO_PTR                  p_lic_handle;
	VO_VIDEO_BUFFER         *p_out_vbuf;
#endif

#if USE_CODEC_NEW_API
	pthread_t thread_id;
	pthread_mutex_t inputStreamMutex;
	pthread_mutex_t getPicMutex;
// 	pthread_mutex_t outputMutex;
	pthread_cond_t inputStreamCond;
	pthread_cond_t getPicCond;
	volatile VO_S32 runCMD;
	volatile VO_S32 bRunning;
#endif

	void *p_dec;
}H265_DEC_FRONT;

VO_U32 FreeSliceBuf( H265_DEC_SLICE *p_slice, USER_OP_ALL* p_user_op_all );

extern VO_U32 CreateH265Decoder(void **p_handle, VO_CODEC_INIT_USERDATA * p_user_op, VO_U32 codec_id);
extern VO_U32 DeleteH265Decoder(void *p_handle);
extern VO_S32 DecodeRawVideo(H265_DEC* p_dec, VO_U8* p_inbuf, VO_S32 buf_len);
extern VO_VOID GetDisPic(H265_DEC *p_dec, VO_VIDEO_BUFFER * p_out_vbuf, VO_VIDEO_OUTPUTINFO * p_out_vbuf_info);
extern VO_S32 ConfigDecoder(H265_DEC* p_dec,  H265_DEC_SLICE *p_slice, H265_DEC_SPS *p_sps);


#endif//__VOH265DECODER_H__