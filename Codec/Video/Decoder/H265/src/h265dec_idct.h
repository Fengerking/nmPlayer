
#ifndef _VONH265DECIDCT_H
#define _VONH265DECIDCT_H

#ifdef __cplusplus
extern  "C" {
#endif
#include "h265dec_config.h"
#include "h265dec_porting.h"

#define BITDEPTH

#define SHIFT_INV_1ST          7	// Shift after first inverse transform stage
#define SHIFT_INV_2ND         12	// Shift after second inverse transform stage

#define SCALING_LIST_NUM      6	// list number for quantization matrix
#define SCALING_LIST_REM_NUM  6	//  remainder of QP/6
  enum SCALING_LIST_SIZE {
    SCALING_LIST_4x4 = 0,
    SCALING_LIST_8x8,
    SCALING_LIST_16x16,
    SCALING_LIST_32x32,
    SCALING_LIST_SIZE_NUM
  };



#if IDCT_ASM_ENABLED

  VO_VOID IDST4X4ASMV7(const VO_S16 *p_invtransform_buff,
                    const VO_U8 *p_prediction_buff,
                    VO_U8 *p_reconstruction_buff,
                    const VO_U32 reconstruction_stride,
                    const VO_U32 prediction_stride,
                    VO_S16 * tmp_block_array);

  VO_VOID IDCT4X4ASMV7(const VO_S16 *p_invtransform_buff,
                    const VO_U8 *p_prediction_buff,
                    VO_U8 *p_reconstruction_buff,
                    const VO_U32 reconstruction_stride,
                    const VO_U32 prediction_stride,
                    VO_S16 * tmp_block_array);

  VO_VOID IDCT8X8ASMV7(const VO_S16 *p_invtransform_buff,
                    const VO_U8 *p_prediction_buff,
                    VO_U8 *p_reconstruction_buff,
                    const VO_U32 reconstruction_stride,
                    const VO_U32 prediction_stride,
                    VO_S16 * tmp_block_array);

  VO_VOID IDCT16X16ASMV7(const VO_S16 *p_invtransform_buff,
                      const VO_U8 *p_prediction_buff,
                      VO_U8 *p_reconstruction_buff,
                      const VO_U32 reconstruction_stride,
                      const VO_U32 prediction_stride,
                      VO_S16 * tmp_block_array);

  VO_VOID IDCT32X32ASMV7(const VO_S16 *p_invtransform_buff,
                      const VO_U8 *p_prediction_buff,
                      VO_U8 *p_reconstruction_buff,
                      const VO_U32 reconstruction_stride,
					            const VO_U32 prediction_stride,
                      VO_S16 * tmp_block_array);

  VO_VOID IDCT_NZ_1X1_ASMV7(const VO_S16 *invtransformValue,
                            const VO_U8 *p_prediction_buff,
                            VO_U8 *p_reconstruction_buff,
                            const VO_U32 reconstruction_stride,
                            const VO_U32 prediction_stride,
                            VO_S16 * tpyeU32_width)		;

  VO_VOID IDST4X4_1X1_ASMV7(const VO_S16 *invtransformValue, 
                            const VO_U8 *p_perdiction_buff,
                            VO_U8 *p_reconstruction_buff,
                            const VO_U32 reconstruction_stride,
                            const VO_U32 prediction_stride,
                            VO_S16 * tmp_block_array);

  VO_VOID IDCT16X16_4X4_ASMV7(const VO_S16 *p_invtransform_buff,
                              const VO_U8 *p_perdiction_buff,
                              VO_U8 *p_reconstruction_buff,
                              const VO_U32 reconstruction_stride,
                              const VO_U32 prediction_stride,
                              VO_S16 * tmp_block_array);

  VO_VOID IDCT32X32_4X4_ASMV7(const VO_S16 *p_invtransform_buff,
                              const VO_U8 *p_perdiction_buff,
                              VO_U8 *p_reconstruction_buff,
                              const VO_U32 reconstruction_stride,
                              const VO_U32 prediction_stride,
                              VO_S16 * tmp_block_array);

#else
  
  VO_VOID IDST4X4(const VO_S16 *p_invtransform_buff,
                const VO_U8 *p_perdiction_buff,
                VO_U8 *p_reconstruction_buff,
                const VO_U32 reconstruction_stride,
                const VO_U32 prediction_stride,
                VO_S16 * tmp_block_array);

  VO_VOID IDCT4X4(const VO_S16 *p_invtransform_buff,
                const VO_U8 *p_perdiction_buff,
                VO_U8 *p_reconstruction_buff,
                const VO_U32 reconstruction_stride,
                const VO_U32 prediction_stride,
                VO_S16 * tmp_block_array);

  VO_VOID IDCT8X8(const VO_S16 *p_invtransform_buff, 
                const VO_U8 *p_perdiction_buff,
                VO_U8 *p_reconstruction_buff,
                const VO_U32 reconstruction_stride,
                const VO_U32 prediction_stride,
                VO_S16 * tmp_block_array);

  VO_VOID IDCT16X16(const VO_S16 *p_invtransform_buff,
                  const VO_U8 *p_perdiction_buff,
                  VO_U8 *p_reconstruction_buff,
                  const VO_U32 reconstruction_stride,
                  const VO_U32 prediction_stride,
                  VO_S16 * tmp_block_array);


  VO_VOID IDCT32X32(const VO_S16 * p_invtransform_buff,
                    const VO_U8 *p_perdiction_buff,
                     VO_U8 * p_reconstruction_buff,
                     const VO_U32 reconstruction_stride,
                     const VO_U32 prediction_stride,
                     VO_S16 * tmp_block_array);


  VO_VOID IDCT_NZ_1X1(const VO_S16 *p_invtransform_buff,
    const VO_U8 *p_prediction_buff,
    VO_U8 *p_reconstruction_buff,
    const VO_U32 reconstruction_stride,
    const VO_U32 prediction_stride,
    VO_S16 * typeU32_width)		;
  VO_VOID IDST4X4_1X1(const VO_S16 *p_invtransform_buff,
    const VO_U8 *p_perdiction_buff,
    VO_U8 *p_reconstruction_buff,
    const VO_U32 reconstruction_stride,
    const VO_U32 prediction_stride,
    VO_S16 * tmp_block_array);
  VO_VOID IDCT16X16_4X4(const VO_S16 *p_invtransform_buff,
    const VO_U8 *p_perdiction_buff,
    VO_U8 *p_reconstruction_buff,
    const VO_U32 reconstruction_stride,
    const VO_U32 prediction_stride,
    VO_S16 * tmp_block_array);
  VO_VOID IDCT32X32_4X4(const VO_S16 *p_invtransform_buff,
    const VO_U8 *p_perdiction_buff,
    VO_U8 *p_reconstruction_buff,
    const VO_U32 reconstruction_stride,
    const VO_U32 prediction_stride,
    VO_S16 * tmp_block_array);

#endif



#ifdef __cplusplus
}
#endif
#endif
