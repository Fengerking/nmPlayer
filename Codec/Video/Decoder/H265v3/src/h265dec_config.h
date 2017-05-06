 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265_config.h
    \brief    h265 high level configuration
    \author   Renjie Yu
	\change
*/

#ifndef __VOH265_CONFIG_H__
#define __VOH265_CONFIG_H__
#define K0251                             1           ///< explicitly signal slice_temporal_mvp_enable_flag in non-IDR I Slices
#define STRONG_INTRA_SMOOTHING           1  ///< Enables Bilinear interploation of reference samples instead of 121 filter in intra prediction when reference samples are flat.
#define DEPENDENT_SLICE_SEGMENT_FLAGS   1   ///< K0184: Move dependent_slice_enabled_flag after slice_seq_parameter_set_id in PPS.
#define SAO_LUM_CHROMA_ONOFF_FLAGS       1  ///< J0087: slice-level independent luma/chroma SAO on/off flag 
#define LTRP_IN_SPS                      1  ///< J0116: Include support for signalling LTRP LSBs in the SPS, and index them in the slice header.
#define CHROMA_QP_EXTENSION              1  ///< J0342: Extend mapping table from luma QP to chroma QP, introduce slice-level chroma offsets, apply limits on offset values
#define SIMPLE_LUMA_CBF_CTX_DERIVATION   1  ///< J0303: simplified luma_CBF context derivation
#define REMOVE_ENTROPY_SLICES 1
#define COEF_REMAIN_BIN_REDUCTION        3 ///< J0142: Maximum codeword length of coeff_abs_level_remaining reduced to 32.
                                           ///< COEF_REMAIN_BIN_REDUCTION is also used to indicate the level at which the VLC 
                                           ///< transitions from Golomb-Rice to TU+EG(k)
#define SPS_INTER_REF_SET_PRED      1   ///< K0136: Not send inter_ref_pic_set_prediction_flag for index 0
#define SAVE_BITS_REFPICLIST_MOD_FLAG               1  ///< K0224 Proposal#1: Send ref_pic_list_modification_flag_lX only when NumPocTotalCurr is greater than 1.
#define REMOVE_BURST_IPCM                1  /// Ticket763
#define CU_DQP_TU_EG                     1 ///< J0089: Bin reduction for delta QP coding
#if (CU_DQP_TU_EG)
#define CU_DQP_TU_CMAX 5 //max number bins for truncated unary
#define CU_DQP_EG_k 0 //expgolomb order
#endif

#define RPL_INIT_FIX 1 ///< K0255 2nd part (editorial)

#define L0043_TIMING_INFO 1
#define L0046_CONSTRAINT_FLAGS 1
#define L0255_MOVE_PPS_FLAGS 1
#define L0363_BYTE_ALIGN 1

#define SIGNAL_BITRATE_PICRATE_IN_VPS               1  ///< K0125: Signal bit_rate and pic_rate in VPS
#define VPS_REARRANGE                               1  ///< JCTVC-K0254
#define HLS_ADD_SUBLAYER_ORDERING_INFO_PRESENT_FLAG 1  ///< K0330: Use sub_layer_ordering_info_present_flag in VPS and SPS as a shortcut to signal only one set of values
#define VPS_OPERATING_POINT                         1  ///< K0204 - Operation point added to VPS
#if VPS_OPERATING_POINT
  #define MAX_VPS_NUM_HRD_PARAMETERS                1
  #define MAX_VPS_NUM_HRD_PARAMETERS_ALLOWED_PLUS1  1024
  #define MAX_VPS_NUH_RESERVED_ZERO_LAYER_ID_PLUS1  1
#endif

#define HLS_EXTRA_SLICE_HEADER_BITS                 1  ///< K0210: signal num_extra_slice_header_bits in PPS
#define HLS_MOVE_SPS_PICLIST_FLAGS                  1  ///< K0170: move restricted_ref_pic_lists_flag and lists_modification_present_flag
#define CONDITION_SUBLAYERPROFILEPRESENTFLAG        1  ///< K0125: Condition signalling of sub_layer_profile_present_flag
#define MOVE_SPS_TEMPORAL_ID_NESTING_FLAG           1  ///< K0120: Move sps_temporal_id_nesting_flag and replace sps_reserved_zero_bit
#define HLS_GROUP_SPS_PCM_FLAGS                     1  ///< K0217: Group together syntax elements for PCM in SPS
#define NAL_UNIT_HEADER                  1  ///< J0550: Define nal_unit_header() method
#define REMOVE_NAL_REF_FLAG              1  ///< J0550: Remove nal_ref_flag, and allocate extra bit to reserved bits, and re-order syntax to put reserved bits after nal_unit_type
#define TEMPORAL_ID_PLUS1                1  ///< J0550: Signal temporal_id_plus1 instead of temporal_id in NAL unit, and change reserved_one_5bits
                                            ///<        value to zero
#define REFERENCE_PICTURE_DEFN           1  ///< J0118: Reflect change of defn. of referece picture in semantics of delta_poc_msb_present_flag
#define MOVE_LOOP_FILTER_SLICES_FLAG     1  ///< J0288: Move seq_loop_filter_across_slices_enabled_flag from SPS to PPS
#define SPLICING_FRIENDLY_PARAMS         1  ///< J0108: Remove rap_pic_id and move no_output_prior_pic_flag

#define  SKIP_FLAG                       1  ///< J0336: store skip flag

#define PPS_TS_FLAG                      1  ///< J0184: move transform_skip_enabled_flag from SPS to PPS
#if PPS_TS_FLAG
#define TS_FLAT_QUANTIZATION_MATRIX      1  ///< I0408: set default quantization matrix to be flat if TS is enabled in PPS
#endif
#define INTER_TRANSFORMSKIP              1  ///< J0237: inter transform skipping (inter-TS)
#define INTRA_TRANSFORMSKIP_FAST         1  ///< J0572: fast encoding for intra transform skipping

#define REMOVAL_8x2_2x8_CG               1  ///< J0256: removal of 8x2 / 2x8 coefficient groups
#define REF_IDX_BYPASS                   1  ///< J0098: bypass coding starting from the second bin for reference index

#define RECALCULATE_QP_ACCORDING_LAMBDA  1  ///< J0242: recalculate QP value according to lambda value
#define TU_ZERO_CBF_RDO                  1  ///< J0241: take the bits to represent zero cbf into consideration when doing TU RDO
#define REMOVE_NUM_GREATER1              1  ///< J0408: numGreater1 removal and ctxset decision with c1 

#define INTRA_TRANS_SIMP                 1  ///< J0035: Use DST for 4x4 luma intra TU's (regardless of the intra prediction direction)

#define J0234_INTER_RPS_SIMPL            1  ///< J0234: Do not signal delta_idx_minus1 when building the RPS-list in SPS
#define NUM_WP_LIMIT                     1  ///< J0571: number of total signalled weight flags <=24
#define DISALLOW_BIPRED_IN_8x4_4x8PUS    1  ///< J0086: disallow bi-pred for 8x4 and 4x8 inter PUs
#define SAO_SINGLE_MERGE                 1  ///< J0355: Single SAO merge flag for all color components (per Left and Up merge)
#define SAO_TYPE_SHARING                 1  ///< J0045: SAO types, merge left/up flags are shared between Cr and Cb
#define SAO_TYPE_CODING                  1  ///< J0268: SAO type signalling using 1 ctx on/off flag + 1 bp BO/EO flag + 2 bp bins for EO class
#define SAO_MERGE_ONE_CTX                1  ///< J0041: SAO merge left/up flags share the same ctx
#define SAO_ABS_BY_PASS                  1  ///< J0043: by pass coding for SAO magnitudes 
#define SAO_LCU_BOUNDARY                 1  ///< J0139: SAO parameter estimation using non-deblocked pixels for LCU bottom and right boundary areas
#define MODIFIED_CROSS_SLICE             1  ///< J0266: SAO slice boundary control for GDR
#define CU_DQP_ENABLE_FLAG               1  ///< J0220: cu_qp_delta_enabled_flag in PPS
#define REMOVE_ZIGZAG_SCAN               1  ///< J0150: removal of zigzag scan

#define TRANS_SPLIT_FLAG_CTX_REDUCTION   1  ///< J0133: Reduce the context number of transform split flag to 3

#define WP_PARAM_RANGE_LIMIT             1  ///< J0221: Range limit of delta_weight and delta_offset for chroma.
#define J0260 1 ///< Fix in rate control equations

#define SLICE_HEADER_EXTENSION           1  ///< II0235: Slice header extension mechanism

#define MERGE_CLEANUP_AND_K0197     1  //<Code cleanup and K0197: removal of indirect use of A1 and B1 in merging candidate list construction.

#define REMOVE_NSQT 1 ///< Disable NSQT-related code
#define REMOVE_LMCHROMA 1 ///< Disable LM_Chroma-related code
#define REMOVE_FGS 1 ///< Disable fine-granularity slices code
#define REMOVE_ALF 1 ///< Disable ALF-related code
#define REMOVE_APS 1 ///< Disable APS-related code

#define PREVREFPIC_DEFN                  0  ///< J0248: Shall be set equal to 0! (prevRefPic definition reverted to CD definition)
#define BYTE_ALIGNMENT                   1  ///< I0330: Add byte_alignment() procedure to end of slice header

#define SBH_THRESHOLD                    4  ///< I0156: value of the fixed SBH controlling threshold
  
#define SEQUENCE_LEVEL_LOSSLESS           0  ///< H0530: used only for sequence or frame-level lossless coding

#define DISABLING_CLIP_FOR_BIPREDME         1  ///< Ticket #175
  
#define C1FLAG_NUMBER               8 // maximum number of largerThan1 flag coded in one chunk :  16 in HM5
#define C2FLAG_NUMBER               1 // maximum number of largerThan2 flag coded in one chunk:  16 in HM5 

#define REMOVE_SAO_LCU_ENC_CONSTRAINTS_3 1  ///< disable the encoder constraint that conditionally disable SAO for chroma for entire slice in interleaved mode

#define SAO_SKIP_RIGHT                   1  ///< H1101: disallow using unavailable pixel during RDO

#define SAO_ENCODING_CHOICE              1  ///< I0184: picture early termination
#define PICTURE_SAO_RDO_FIX              0  ///< J0097: picture-based SAO optimization fix
#if SAO_ENCODING_CHOICE
#define SAO_ENCODING_RATE                0.75
#define SAO_ENCODING_CHOICE_CHROMA       1 ///< J0044: picture early termination Luma and Chroma are handled separatenly
#if SAO_ENCODING_CHOICE_CHROMA
#define SAO_ENCODING_RATE_CHROMA         0.5
#endif
#endif

#define MAX_NUM_SPS                32
#define MAX_NUM_PPS                256
#define MAX_NUM_APS                32         //< !!!KS: number not defined in WD yet

#define MRG_MAX_NUM_CANDS_SIGNALED         5   //<G091: value of maxNumMergeCand signaled in slice header 

#define WEIGHTED_CHROMA_DISTORTION  1   ///< F386: weighting of chroma for RDO
#define RDOQ_CHROMA_LAMBDA          1   ///< F386: weighting of chroma for RDOQ
#define ALF_CHROMA_LAMBDA           1   ///< F386: weighting of chroma for ALF
#define SAO_CHROMA_LAMBDA           1   ///< F386: weighting of chroma for SAO

#define MIN_SCAN_POS_CROSS          4

#define FAST_BIT_EST                1   ///< G763: Table-based bit estimation for CABAC

#define MLS_GRP_NUM                         64     ///< G644 : Max number of coefficient groups, max(16, 64)
#define MLS_CG_SIZE                         4      ///< G644 : Coefficient group size of 4x4

#define ADAPTIVE_QP_SELECTION               1      ///< G382: Adaptive reconstruction levels, non-normative part for adaptive QP selection
#if ADAPTIVE_QP_SELECTION
#define ARL_C_PRECISION                     7      ///< G382: 7-bit arithmetic precision
#define LEVEL_RANGE                         30     ///< G382: max coefficient level in statistics collection
#endif

#if REMOVE_NSQT
#define NS_HAD                               0
#else
#define NS_HAD                               1
#endif

#define APS_BITS_FOR_SAO_BYTE_LENGTH 12           
#define APS_BITS_FOR_ALF_BYTE_LENGTH 8

#define HHI_RQT_INTRA_SPEEDUP             1           ///< tests one best mode with full rqt
#define HHI_RQT_INTRA_SPEEDUP_MOD         0           ///< tests two best modes with full rqt

#if HHI_RQT_INTRA_SPEEDUP_MOD && !HHI_RQT_INTRA_SPEEDUP
#error
#endif

#define VERBOSE_RATE 0 ///< Print additional rate information in encoder

#define AMVP_DECIMATION_FACTOR            4

#define SCAN_SET_SIZE                     16
#define LOG2_SCAN_SET_SIZE                4

#define FAST_UDI_MAX_RDMODE_NUM               35          ///< maximum number of RD comparison in fast-UDI estimation loop 

#define ZERO_MVD_EST                          0           ///< Zero Mvd Estimation in normal mode

#define NUM_INTRA_MODE 36
#if !REMOVE_LM_CHROMA
#define LM_CHROMA_IDX  35
#endif

#define IBDI_DISTORTION                0           ///< enable/disable SSE modification when IBDI is used (JCTVC-D152)
#define FIXED_ROUNDING_FRAME_MEMORY    0           ///< enable/disable fixed rounding to 8-bitdepth of frame memory when IBDI is used  

#define WRITE_BACK                      1           ///< Enable/disable the encoder to replace the deltaPOC and Used by current from the config file with the values derived by the refIdc parameter.
#define AUTO_INTER_RPS                  1           ///< Enable/disable the automatic generation of refIdc from the deltaPOC and Used by current from the config file.
#define PRINT_RPS_INFO                  0           ///< Enable/disable the printing of bits used to send the RPS.
                                                    // using one nearest frame as reference frame, and the other frames are high quality (POC%4==0) frames (1+X)
                                                    // this should be done with encoder only decision
                                                    // but because of the absence of reference frame management, the related code was hard coded currently

#define RVM_VCEGAM10_M 4


#define FAST_UDI_USE_MPM 1

#define RDO_WITHOUT_DQP_BITS              0           ///< Disable counting dQP bits in RDO-based mode decision

#define FULL_NBIT 0 ///< When enabled, does not use g_uiBitIncrement anymore to support > 8 bit data

#define AD_HOC_SLICES_FIXED_NUMBER_OF_LCU_IN_SLICE      1          ///< OPTION IDENTIFIER. mode==1 -> Limit maximum number of largest coding tree blocks in a slice
#define AD_HOC_SLICES_FIXED_NUMBER_OF_BYTES_IN_SLICE    2          ///< OPTION IDENTIFIER. mode==2 -> Limit maximum number of bins/bits in a slice
#define AD_HOC_SLICES_FIXED_NUMBER_OF_TILES_IN_SLICE    3
#define HM9_NALU_TYPES 1
#define DEPENDENT_SLICES       1 ///< JCTVC-I0229
// Dependent slice options
#define SHARP_FIXED_NUMBER_OF_LCU_IN_DEPENDENT_SLICE            1          ///< OPTION IDENTIFIER. Limit maximum number of largest coding tree blocks in an dependent slice
#define SHARP_MULTIPLE_CONSTRAINT_BASED_DEPENDENT_SLICE         2          ///< OPTION IDENTIFIER. Limit maximum number of bins/bits in an dependent slice
#if DEPENDENT_SLICES
#define FIXED_NUMBER_OF_TILES_IN_DEPENDENT_SLICE          3 // JCTVC-I0229
#endif

#define LOG2_MAX_NUM_COLUMNS_MINUS1        7
#define LOG2_MAX_NUM_ROWS_MINUS1           7
#define LOG2_MAX_COLUMN_WIDTH              13
#define LOG2_MAX_ROW_HEIGHT                13

#define MATRIX_MULT                             0   // Brute force matrix multiplication instead of partial butterfly

#define REG_DCT 65535

#define AMP_SAD                               1           ///< dedicated SAD functions for AMP
#define AMP_ENC_SPEEDUP                       1           ///< encoder only speed-up by AMP mode skipping
#if AMP_ENC_SPEEDUP
#define AMP_MRG                               1           ///< encoder only force merge for AMP partition (no motion search for AMP)
#endif

#define SCALING_LIST_OUTPUT_RESULT    0 //JCTVC-G880/JCTVC-G1016 quantization matrices

#define CABAC_INIT_PRESENT_FLAG     1

//=========================================================
//++ add by rayman
#define PRED_CACHE_STRIDE           80

#define RESTRICT_INTRA_BOUNDARY_SMOOTHING    1  ///< K0380, K0186 
#define LINEBUF_CLEANUP               1 ///< K0101

//#define LCU_NUM_IN_PIC 30 //TBD

#define TILECOL 4
#define TILEROW 3

#define     MAX_CU_DEPTH            7                           // log2(LCUSize)
#define     MAX_CU_SIZE             (1<<(MAX_CU_DEPTH))         // maximum allowable size of CU
#define     MIN_PU_SIZE             4
#define     MAX_NUM_SPU_W           64//(MAX_CU_SIZE/MIN_PU_SIZE)   // maximum number of SPU in horizontal line
#define MAX_NUM_REF_PICS            16          ///< max. number of pictures used for reference
#define MAX_NUM_REF_LC              MAX_NUM_REF_PICS  // TODO: remove this macro definition (leftover from combined list concept)

#define MAX_TLAYER                  8           ///< max number of temporal layer

// MERGE
#define MRG_MAX_NUM_CANDS           5
#define SAO_MAX_DEPTH 4
#define NUM_DOWN_PART 4
#define SAO_BO_BITS                   5
#define LUMA_GROUP_NUM                (1<<SAO_BO_BITS)
#define MAX_NUM_SAO_OFFSETS           4
#define MAX_NUM_SAO_CLASS             33

//--
//when change following macros, make sure they are same as that in h265dec_ASM_config.h
#ifdef ASM_OPT                          //ASM_OPT is controlled in Makefile or project setting

#ifdef ARCH_X86  //will remove it when ARCH_X86 is done

#define MC_ASM_ENABLED                1
#define IDCT_ASM_ENABLED              0 //TODO
#define DEBLOCK_ASM_ENABLED           1 
#define SAO_ASM_ENABLED               1
#ifdef _IOS
#define USE_INLINE_ASM                0
#else
#define USE_INLINE_ASM                0 //TODO
#endif
#define INTRA_ASM_ENABLED             0 //TODO

#else

#define MC_ASM_ENABLED                1
#define IDCT_ASM_ENABLED              1
#define DEBLOCK_ASM_ENABLED           1
#define SAO_ASM_ENABLED               1
#ifdef _IOS
#define USE_INLINE_ASM                0
#else
#define USE_INLINE_ASM                1
#endif
#define INTRA_ASM_ENABLED             1

#endif

#else
#define MC_ASM_ENABLED                0
#define IDCT_ASM_ENABLED              0
#define DEBLOCK_ASM_ENABLED           0
#define SAO_ASM_ENABLED               0
#endif

#define ENABLE_THD_INFO               0
//#define REMOVE_CACHE_BUFF             1
#define PAD_BUFF_ENABLED              1

//wpp
#define MAX_ENTRY_SIZE 72
#define MAX_EMULATE_SIZE 4096
#define USE_EMULATE_CODE    1

//added by Harry
#define USE_CODEC_NEW_API 1
#define MAX_DROP_FRAME_INTERVAL 1	//max num of frames can continually dropped
//enable fast mode when dis list 1/x empty
#define AUTO_FASTMODE_SAO	5	//20%
#define AUTO_FASTMODE_DBK	3	//33%
#define AUTO_FASTMODE_DROP	2	//50%

#define ENABLE_DROP_FRAME	1
#define MAX_THREAD_NUM2 8
#define USE_FRAME_THREAD 0
#if USE_CODEC_NEW_API
#define MAX_FRAME_INPUT 16
#else
#define MAX_FRAME_INPUT 100
#endif

#define WPP_SKIP_BYTE_OFFSET 1


#if USE_FRAME_THREAD
#include "h265dec_RVThread.h"
#include "h265dec_frameThd.h"
#endif

/////////////////////////////////////////////////
//3D-WAVE

#define USE_3D_WAVE_THREAD 1

#if USE_3D_WAVE_THREAD
#include "h265dec_RVThread.h"
#include "h265dec_wave3d.h"
#endif


#if defined(_VOLOG_ERROR) 
#if defined(ANDROID)
//#include <pthread.h>
#include <android/log.h>
#define LOG_TAG "_YYHEVC"
#define LOGI(...) ((int)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#elif defined(WIN32) && defined(_DEBUG)
#include <stdlib.h>
#include <stdarg.h>
#include <windows.h>
#define STRSAFE_NO_DEPRECATE 
#include <strsafe.h>
// #define LOGI printf
static void LOGI(const char *lpszFormat,...)
{
	int nBuf;
	char szBuffer[512] = "_YYHEVC log: ";
	va_list args;
	va_start(args,lpszFormat);
	nBuf = strlen(szBuffer);
	nBuf = _vsnprintf(szBuffer + nBuf, sizeof(szBuffer) - nBuf, lpszFormat, args);

	OutputDebugString(szBuffer);
	va_end(args);
};
#else//_LINUX_ANDROID
#define LOGI(...) 
#endif//_LINUX_ANDROID
#else
#define LOGI(...) 
#endif
#define VOH265ERROR(a)	\
{\
	LOGI("error:%x\n",a);\
	return a;\
}

#endif //__VOH265_CONFIG_H__