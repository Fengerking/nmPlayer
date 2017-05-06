#ifndef VOCABACTABLE_H
#define VOCABACTABLE_H
#include "h265dec_porting.h"
#include "h265_decoder.h"


// ====================================================================================================================
// Constants
// ====================================================================================================================

#define MAX_NUM_CTX_MOD             512       ///< maximum number of supported contexts

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


// ====================================================================================================================
// Tables
// ====================================================================================================================

// initial probability for cu_transquant_bypass flag
static const VO_U8
INIT_CU_TRANSQUANT_BYPASS_FLAG[3][NUM_CU_TRANSQUANT_BYPASS_FLAG_CTX] =
{
  { 154 }, 
  { 154 }, 
  { 154 }, 
};

// initial probability for split flag
static const VO_U8 
INIT_SPLIT_FLAG[3][NUM_SPLIT_FLAG_CTX] =  
{ 

  { 107,  139,  126, },
  { 107,  139,  126, }, 
  { 139,  141,  157, }, 
};

static const VO_U8 
INIT_SKIP_FLAG[3][NUM_SKIP_FLAG_CTX] =  
{
  { 197,  185,  201, }, 
  { 197,  185,  201, }, 
  { CNU,  CNU,  CNU, }, 
};
#if 0
static const VO_U8 
INIT_ALF_CTRL_FLAG[3][NUM_ALF_CTRL_FLAG_CTX] = 
{
  { 102, }, 
  { 102, }, 
  { 118, }, 
};
#endif
static const VO_U8 
INIT_MERGE_FLAG_EXT[3][NUM_MERGE_FLAG_EXT_CTX] = 
{
  { 154, }, 
  { 110, }, 
  { CNU, }, 
};

static const VO_U8 
INIT_MERGE_IDX_EXT[3][NUM_MERGE_IDX_EXT_CTX] =  
{
  { 137, }, 
  { 122, }, 
  { CNU, }, 
};

static const VO_U8 
INIT_PART_SIZE[3][NUM_PART_SIZE_CTX] =  
{
  { 154,  139,  CNU,  CNU, }, 
  { 154,  139,  CNU,  CNU, }, 
  { 184,  CNU,  CNU,  CNU, }, 
};

static const VO_U8 
INIT_CU_AMP_POS[3][NUM_CU_AMP_CTX] =  
{
  { 154, }, 
  { 154, }, 
  { CNU, }, 
};

static const VO_U8 
INIT_PRED_MODE[3][NUM_PRED_MODE_CTX] = 
{
  { 134, }, 
  { 149, }, 
  { CNU, }, 
};

static const VO_U8 
INIT_INTRA_PRED_MODE[3][NUM_ADI_CTX] = 
{
  { 183, }, 
  { 154, }, 
  { 184, }, 
};

static const VO_U8 
INIT_CHROMA_PRED_MODE[3][NUM_CHROMA_PRED_CTX] = 
{
  { 152,  139, }, 
  { 152,  139, }, 
  {  63,  139, }, 
};

static const VO_U8 
INIT_INTER_DIR[3][NUM_INTER_DIR_CTX] = 
{
  {  95,   79,   63,   31,  31, }, 
  {  95,   79,   63,   31,  31, }, 
  { CNU,  CNU,  CNU,  CNU, CNU, }, 
};

static const VO_U8 
INIT_MVD[3][NUM_MV_RES_CTX] =  
{
  { 169,  198, }, 
  { 140,  198, }, 
  { CNU,  CNU, }, 
};

#if REF_IDX_BYPASS
static const VO_U8 
INIT_REF_PIC[3][NUM_REF_NO_CTX] =  
{
  { 153,  153 }, 
  { 153,  153 }, 
  { CNU,  CNU }, 
};
#else
static const VO_U8 
INIT_REF_PIC[3][NUM_REF_NO_CTX] =  
{
  { 153,  153,  168,  CNU, }, 
  { 153,  153,  139,  CNU, }, 
  { CNU,  CNU,  CNU,  CNU, }, 
};
#endif

static const VO_U8 
INIT_DQP[3][NUM_DELTA_QP_CTX] = 
{
  { 154,  154,  154, }, 
  { 154,  154,  154, }, 
  { 154,  154,  154, }, 
};

static const VO_U8 
INIT_QT_CBF[3][2*NUM_QT_CBF_CTX] =  
{
  { 153,  111,  CNU,  CNU,  CNU,  149,   92,  167,  CNU,  CNU, }, 
  { 153,  111,  CNU,  CNU,  CNU,  149,  107,  167,  CNU,  CNU, }, 
  { 111,  141,  CNU,  CNU,  CNU,   94,  138,  182,  CNU,  CNU, }, 
};

static const VO_U8 
INIT_QT_ROOT_CBF[3][NUM_QT_ROOT_CBF_CTX] = 
{
  {  79, }, 
  {  79, }, 
  { CNU, }, 
};

static const VO_U8 
INIT_LAST[3][2*NUM_CTX_LAST_FLAG_XY] =  
{
  { 125,  110,  124,  110,   95,   94,  125,  111,  111,   79,  125,  126,  111,  111,   79,
    108,  123,   93,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU, 
  }, 
  { 125,  110,   94,  110,   95,   79,  125,  111,  110,   78,  110,  111,  111,   95,   94,
    108,  123,  108,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,
  }, 
  { 110,  110,  124,  125,  140,  153,  125,  127,  140,  109,  111,  143,  127,  111,   79, 
    108,  123,   63,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU, 
  }, 
};

static const VO_U8 
INIT_SIG_CG_FLAG[3][2 * NUM_SIG_CG_FLAG_CTX] =  
{
  { 121,  140,  
    61,  154, 
  }, 
  { 121,  140, 
    61,  154, 
  }, 
  {  91,  171,  
    134,  141, 
  }, 
};

static const VO_U8 
INIT_SIG_FLAG[3][NUM_SIG_FLAG_CTX] = 
{
#if REMOVAL_8x2_2x8_CG
  { 170,  154,  139,  153,  139,  123,  123,   63,  124,  166,  183,  140,  136,  153,  154,  166,  183,  140,  136,  153,  154,  166,  183,  140,  136,  153,  154,  170,  153,  138,  138,  122,  121,  122,  121,  167,  151,  183,  140,  151,  183,  140,  }, 
  { 155,  154,  139,  153,  139,  123,  123,   63,  153,  166,  183,  140,  136,  153,  154,  166,  183,  140,  136,  153,  154,  166,  183,  140,  136,  153,  154,  170,  153,  123,  123,  107,  121,  107,  121,  167,  151,  183,  140,  151,  183,  140,  }, 
  { 111,  111,  125,  110,  110,   94,  124,  108,  124,  107,  125,  141,  179,  153,  125,  107,  125,  141,  179,  153,  125,  107,  125,  141,  179,  153,  125,  140,  139,  182,  182,  152,  136,  152,  136,  153,  136,  139,  111,  136,  139,  111,  }, 
#else
  { 170,      154,  139,  153,  139,  123,  123,   63,  124,     153, 153, 152, 152, 152, 137, 152, 137, 137,      166,  183,  140,  136,  153,  154,      170,     153, 138, 138,  122, 121,   122, 121,   167,     153,  167,  136,  121,  122,  136,  121,  122,   91,      151,  183,  140,  }, 
  { 155,      154,  139,  153,  139,  123,  123,   63,  153,     153, 153, 152, 152, 152, 137, 152, 137, 122,      166,  183,  140,  136,  153,  154,      170,     153, 123, 123,  107, 121,   107, 121,   167,     153,  167,  136,  149,  107,  136,  121,  122,   91,      151,  183,  140,  }, 
  { 111,      111,  125,  110,  110,   94,  124,  108,  124,     139, 139, 139, 168, 124, 138, 124, 138, 107,      107,  125,  141,  179,  153,  125,      140,     139, 182, 182,  152, 136,   152, 136,   153,     182,  137,  149,  192,  152,  224,  136,   31,  136,      136,  139,  111,  }, 
#endif
};

static const VO_U8 
INIT_ONE_FLAG[3][NUM_ONE_FLAG_CTX] = 
{
  { 154,  196,  167,  167,  154,  152,  167,  182,  182,  134,  149,  136,  153,  121,  136,  122,  169,  208,  166,  167,  154,  152,  167,  182, }, 
  { 154,  196,  196,  167,  154,  152,  167,  182,  182,  134,  149,  136,  153,  121,  136,  137,  169,  194,  166,  167,  154,  167,  137,  182, }, 
  { 140,   92,  137,  138,  140,  152,  138,  139,  153,   74,  149,   92,  139,  107,  122,  152,  140,  179,  166,  182,  140,  227,  122,  197, }, 
};

static const VO_U8 
INIT_ABS_FLAG[3][NUM_ABS_FLAG_CTX] =  
{
  { 107,  167,   91,  107,  107,  167, }, 
  { 107,  167,   91,  122,  107,  167, }, 
  { 138,  153,  136,  167,  152,  152, }, 
};

static const VO_U8 
INIT_MVP_IDX[3][NUM_MVP_IDX_CTX] =  
{
  { 168,  CNU, }, 
  { 168,  CNU, }, 
  { CNU,  CNU, }, 
};
#if 0
static const VO_U8 
INIT_ALF_FLAG[3][NUM_ALF_FLAG_CTX] = 
{
  { 153, }, 
  { 153, }, 
  { 153, }, 
};

static const VO_U8 
INIT_ALF_UVLC[3][NUM_ALF_UVLC_CTX] = 
{
  { 154,  154, }, 
  { 154,  154, }, 
  { 140,  154, }, 
};

static const VO_U8 
INIT_ALF_SVLC[3][NUM_ALF_SVLC_CTX] =  
{
  { 141,  154,  159, }, 
  { 141,  154,  189, }, 
  { 187,  154,  159, }, 
};
#endif
#if !SAO_ABS_BY_PASS
static const VO_U8 
INIT_SAO_UVLC[3][NUM_SAO_UVLC_CTX] =  
{
  { 200,  140, }, 
  { 185,  140, }, 
  { 143,  140, }, 
};
#endif
#if SAO_MERGE_ONE_CTX
static const VO_U8 
INIT_SAO_MERGE_FLAG[3][NUM_SAO_MERGE_FLAG_CTX] = 
{
  { 153,  }, 
  { 153,  }, 
  { 153,  }, 
};
#else
static const VO_U8 
INIT_SAO_MERGE_LEFT_FLAG[3][NUM_SAO_MERGE_LEFT_FLAG_CTX] = 
{
#if SAO_SINGLE_MERGE
  { 153, }, 
  { 153, }, 
  { 153, }, 
#else
  { 153,  153,  153, }, 
  { 153,  153,  153, }, 
  { 153,  153,  153, }, 
#endif
};

static const VO_U8 
INIT_SAO_MERGE_UP_FLAG[3][NUM_SAO_MERGE_UP_FLAG_CTX] = 
{
  { 153, }, 
  { 153, }, 
  { 175, }, 
};
#endif

static const VO_U8 
INIT_SAO_TYPE_IDX[3][NUM_SAO_TYPE_IDX_CTX] = 
{
  { 160, },
  { 185, },
  { 200, },
};

#if TRANS_SPLIT_FLAG_CTX_REDUCTION
static const VO_U8
INIT_TRANS_SUBDIV_FLAG[3][NUM_TRANS_SUBDIV_FLAG_CTX] =
{
  { 224,  167,  122, },
  { 124,  138,   94, },
  { 153,  138,  138, },
};
#else
static const VO_U8 
INIT_TRANS_SUBDIV_FLAG[3][NUM_TRANS_SUBDIV_FLAG_CTX] = 
{
{ CNU,  153,  138,  138,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU, }, 
{ CNU,  124,  138,   94,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU, }, 
{ CNU,  224,  167,  122,  CNU,  CNU,  CNU,  CNU,  CNU,  CNU, }, 
};
#endif

static const VO_U8
INIT_TRANSFORMSKIP_FLAG[3][2*NUM_TRANSFORMSKIP_FLAG_CTX] = 
{
  { 139,  139}, 
  { 139,  139}, 
  { 139,  139}, 
};

#endif