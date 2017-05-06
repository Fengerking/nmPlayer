#ifndef _DEQUANTCOEF_H
#define _DEQUANTCOEF_H

// #include "h265dec_idct.h"
// #include "h265dec_bits.h"
#include "h265_decoder.h"

#define SCALING_LIST_START_VALUE 8 ///< start value for dpcm mode
#define MAX_MATRIX_COEF_NUM 64     ///< max coefficient number for quantization matrix
#define MAX_MATRIX_SIZE_NUM 8      ///< max size number for quantization matrix
#define SCALING_LIST_DC 16         ///< default DC value

#define     MAX_CU_DEPTH            7                           // log2(LCUSize)



//  //m_dequantCoef
//extern VO_S32 * g_aiDequantCoef[SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM][SCALING_LIST_REM_NUM]; ///< array of dequantization matrix coefficient 4x4  
//extern VO_S16 * g_aiDequantCoef[SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM][SCALING_LIST_REM_NUM]; ///< array of dequantization matrix coefficient 4x4  



#define FLAT_4x4_DSL 1 ///< Use flat 4x4 default scaling list (see notes on K0203)


// typedef struct TComScalingList
// {
//     int      m_scalingListDC               [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM]; //!< the DC value of the matrix coefficient for 16x16
//     //Bool     m_useDefaultScalingMatrixFlag [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM]; //!< UseDefaultScalingMatrixFlag
//     unsigned int      m_refMatrixId                 [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM]; //!< RefMatrixID
//     //Bool     m_scalingListPresentFlag;                                                //!< flag for using default matrix
//     //UInt     m_predMatrixId                [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM]; //!< reference list index
//     int      *m_scalingListCoef            [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM]; //!< quantization matrix
//     unsigned char     m_useTransformSkip;                                                      //!< transform skipping flag for setting default scaling matrix for 4x4
// } stScalingList;

//stScalingList stScalingListEntity;
VO_VOID alloc_mMulLevelScale(H265_DEC_SLICE * p_slice);
VO_VOID free_mMulLevelScale(H265_DEC_SLICE * p_slice);
VO_VOID setMMulLevelScale(H265_DEC_SLICE * p_slice,stScalingList * pstScalingListEntity);

VO_VOID alloc_ScalingList(H265_DEC_SLICE * p_slice, stScalingList *pstScalingListEntity);
VO_VOID free_ScalingList(H265_DEC *p_dec, stScalingList *pstScalingListEntity);
VO_VOID setDefaultScalingList(stScalingList *pstScalingListEntity);
VO_VOID scaling_list_data(stScalingList *pstScalingListEntity,BIT_STREAM *p_bs);






#endif