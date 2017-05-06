#ifndef VOCABACTABLE_H
#define VOCABACTABLE_H
#include "h265dec_porting.h"
#include "h265_decoder.h"
#include "assert.h"

// ====================================================================================================================
// Constants
// ====================================================================================================================

#define MAX_NUM_CTX_MOD             186       ///< maximum number of supported contexts
#define CABAC_TABLE_SIZE_SLICE      52*MAX_NUM_CTX_MOD  

#define NUM_SPLIT_FLAG_CTX            3       ///< number of context models for split flag
#define NUM_SKIP_FLAG_CTX             3       ///< number of context models for skip flag

#define NUM_MERGE_FLAG_EXT_CTX        1       ///< number of context models for merge flag of merge extended
#define NUM_MERGE_IDX_EXT_CTX         1       ///< number of context models for merge index of merge extended

#define NUM_PART_SIZE_CTX             4       ///< number of context models for partition size
#define NUM_CU_AMP_CTX                1       ///< number of context models for partition size (AMP)
#define NUM_PRED_MODE_CTX             1       ///< number of context models for prediction mode

#define NUM_ADI_CTX                   1       ///< number of context models for intra prediction

#define NUM_CHROMA_PRED_CTX           2       ///< number of context models for intra prediction (chroma)
#define NUM_INTER_DIR_CTX             5       ///< number of context models for inter prediction direction
#define NUM_MV_RES_CTX                2       ///< number of context models for motion vector difference

#define NUM_REF_NO_CTX                2       ///< number of context models for reference index
#define NUM_TRANS_SUBDIV_FLAG_CTX     3       ///< number of context models for transform subdivision flags
#define NUM_QT_CBF_CTX                5       ///< number of context models for QT CBF
#define NUM_QT_ROOT_CBF_CTX           1       ///< number of context models for QT ROOT CBF
#define NUM_DELTA_QP_CTX              3       ///< number of context models for dQP

#define NUM_SIG_CG_FLAG_CTX           2       ///< number of context models for MULTI_LEVEL_SIGNIFICANCE

#define NUM_SIG_FLAG_CTX              42      ///< number of context models for sig flag
#define NUM_SIG_FLAG_CTX_LUMA         27      ///< number of context models for luma sig flag
#define NUM_SIG_FLAG_CTX_CHROMA       15      ///< number of context models for chroma sig flag

#define NUM_CTX_LAST_FLAG_XY          15      ///< number of context models for last coefficient position

#define NUM_ONE_FLAG_CTX              24      ///< number of context models for greater than 1 flag
#define NUM_ONE_FLAG_CTX_LUMA         16      ///< number of context models for greater than 1 flag of luma
#define NUM_ONE_FLAG_CTX_CHROMA        8      ///< number of context models for greater than 1 flag of chroma
#define NUM_ABS_FLAG_CTX               6      ///< number of context models for greater than 2 flag
#define NUM_ABS_FLAG_CTX_LUMA          4      ///< number of context models for greater than 2 flag of luma
#define NUM_ABS_FLAG_CTX_CHROMA        2      ///< number of context models for greater than 2 flag of chroma

#define NUM_MVP_IDX_CTX               2       ///< number of context models for MVP index

#define NUM_SAO_MERGE_FLAG_CTX        1       ///< number of context models for SAO merge flags
#define NUM_SAO_TYPE_IDX_CTX          1       ///< number of context models for SAO type index

#define NUM_TRANSFORMSKIP_FLAG_CTX    1       ///< number of context models for transform skipping 
#define NUM_CU_TRANSQUANT_BYPASS_FLAG_CTX  1 
#define CNU                          154      ///< dummy initialization value for unused context models 'Context model Not Used'

#define OFF_SPLIT_FLAG_CTX          ( 0 )
#define OFF_SKIP_FLAG_CTX           ( OFF_SPLIT_FLAG_CTX        +   NUM_SPLIT_FLAG_CTX      )
#define OFF_MERGE_FLAG_EXT_CTX      ( OFF_SKIP_FLAG_CTX         +   NUM_SKIP_FLAG_CTX       )
#define OFF_MERGE_IDX_EXT_CTX       ( OFF_MERGE_FLAG_EXT_CTX    +   NUM_MERGE_FLAG_EXT_CTX  )
#define OFF_PART_SIZE_CTX           ( OFF_MERGE_IDX_EXT_CTX     +   NUM_MERGE_IDX_EXT_CTX   )
#define OFF_CU_AMP_CTX              ( OFF_PART_SIZE_CTX         +   NUM_PART_SIZE_CTX       )
#define OFF_PRED_MODE_CTX           ( OFF_CU_AMP_CTX             +   NUM_CU_AMP_CTX       )
#define OFF_INTRA_PRED_CTX          ( OFF_PRED_MODE_CTX         +   NUM_PRED_MODE_CTX   )
#define OFF_CHROMA_PRED_CTX         ( OFF_INTRA_PRED_CTX        +   NUM_ADI_CTX      )
#define OFF_DELTA_QP_CTX            ( OFF_CHROMA_PRED_CTX       +   NUM_CHROMA_PRED_CTX     )

#define OFF_INTER_DIR_CTX           ( OFF_DELTA_QP_CTX          +   NUM_DELTA_QP_CTX        )
#define OFF_REF_PIC_CTX             ( OFF_INTER_DIR_CTX         +   NUM_INTER_DIR_CTX       )
#define OFF_MVD_CTX                 ( OFF_REF_PIC_CTX           +   NUM_REF_NO_CTX          )
#define OFF_QT_CBF_CTX              ( OFF_MVD_CTX               +   NUM_MV_RES_CTX          )
#define OFF_TRANS_SUBDIV_FLAG_CTX   ( OFF_QT_CBF_CTX            + 2*NUM_QT_CBF_CTX        )
#define OFF_QT_ROOT_CBF_CTX         ( OFF_TRANS_SUBDIV_FLAG_CTX +   NUM_TRANS_SUBDIV_FLAG_CTX        )
#define OFF_SIG_CG_FLAG_CTX         ( OFF_QT_ROOT_CBF_CTX       +   NUM_QT_ROOT_CBF_CTX     )
#define OFF_SIG_FLAG_CTX            ( OFF_SIG_CG_FLAG_CTX       + 2*NUM_SIG_CG_FLAG_CTX     )
#define OFF_LAST_X_CTX              ( OFF_SIG_FLAG_CTX          +   NUM_SIG_FLAG_CTX        )
#define OFF_LAST_Y_CTX              ( OFF_LAST_X_CTX            + 2*NUM_CTX_LAST_FLAG_XY    )
#define OFF_ONE_FLAG_CTX            ( OFF_LAST_Y_CTX            + 2*NUM_CTX_LAST_FLAG_XY    )
#define OFF_ABS_FLAG_CTX            ( OFF_ONE_FLAG_CTX          +   NUM_ONE_FLAG_CTX        )
#define OFF_MVP_IDX_CTX             ( OFF_ABS_FLAG_CTX          +   NUM_ABS_FLAG_CTX        )
//#define OFF_CU_AMP_CTX              ( OFF_MVP_IDX_CTX          +   NUM_MVP_IDX_CTX        )
#define OFF_SAO_MERGE_FLAG_CTX      ( OFF_MVP_IDX_CTX            +   NUM_MVP_IDX_CTX          )
#define OFF_SAO_TYPE_IDX_CTX              ( OFF_SAO_MERGE_FLAG_CTX     +   NUM_SAO_MERGE_FLAG_CTX     )
#define OFF_TRANSFORMSKIP_FLAG_CTX        ( OFF_SAO_TYPE_IDX_CTX       +   NUM_SAO_TYPE_IDX_CTX       )
#define OFF_CU_TRANSQUANT_BYPASS_FLAG_CTX ( OFF_TRANSFORMSKIP_FLAG_CTX + 2*NUM_TRANSFORMSKIP_FLAG_CTX )



VO_VOID voCabacInit( H265_DEC_SLICE* p_slice, VO_S32 entry);
VO_VOID voCabacInitExt( H265_DEC_SLICE* p_slice, VO_S32 entry);


static VOINLINE VO_VOID voCabacDecodePCMAlignBits(BIT_STREAM* p_bs)
{
  VO_S32 iNum=p_bs->bit_pos & 0x7;
  VO_U32 uiBit = 0;
  UPDATE_CACHE(p_bs);
  uiBit=GetBits(p_bs,iNum);
}

VO_U32 ParseSaoTypeIdx ( H265_DEC_CABAC *p_cabac/*,*/
	/*BIT_STREAM *p_bs*/ );

VO_U32 ParserSaoOffsetAbs( H265_DEC_CABAC *p_cabac/*,*/
	/*BIT_STREAM *p_bs*/ );

VO_U32 GetCtxSplitInc( H265_DEC_SLICE* p_slice, 
	VO_U32 depth ,
	VO_S32 x,
	VO_S32 y);

VO_U32 GetCtxSkipFlag( H265_DEC_SLICE* p_slice, 
	VO_S32 x,
	VO_S32 y);

VO_U32 parseMergeIndex ( H265_DEC_SLICE* p_slice,
	H265_DEC_CABAC *p_cabac);

VO_U32 parseRefFrmIdx( H265_DEC_SLICE *p_slice,
	H265_DEC_CABAC *p_cabac,
	/*BIT_STREAM *p_bs,*/
	VO_U32 max_ref_num );

extern VOINLINE VO_BOOL DeriveZsBlockAvail( H265_DEC_SLICE * p_slice, 
    const VO_S32 xCb, 
    const VO_S32 yCb, 
    const VO_S32 xNbY, 
    const VO_S32 yNbY);

VO_VOID voCabacTableInit(VO_S32 iQp,VO_U32 nSlice,VO_U8* table);
#endif