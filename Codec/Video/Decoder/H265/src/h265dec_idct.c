
#include "h265dec_idct.h"
#include "h265dec_utils.h"

#ifndef REG_DCT
#define REG_DCT 65535
#endif

// Function point for IDCT which is pointed to actual block size transform.
typedef VO_VOID (*P_FUNCTION_POINT_FOR_IDCT) (const VO_S16 * p_src_data, 
                                           const VO_U8 * p_prediction_data, 
                                           VO_U8 * p_reconstruction_data,
                                           const VO_U32 reconstruction_stride,
                                           const VO_U32 prediction_stride);

#if !defined(_IOS) && IDCT_ASM_ENABLED						// be used for assembly function
/*
P_FUNCTION_POINT_FOR_IDCT IDCT_function_pointer_array[8] = { 
	IDCT4X4ASMV7, IDCT8X8ASMV7, 0, IDCT16X16ASMV7, IDST4X4ASMV7, 0, 0, 0 };*/

// the coefficient array in idct 4x4 size
const VO_S16 kg_IDCT_coef_for_t4_asm[4] = { 64, 83, 64, 36 };

// the coefficient array in idct 8x8 size
const VO_S16 kg_IDCT_coef_for_t8_asm[8] = {
  89, 75, 50, 18, 64, 83, 64, 36
};

// the coefficient array in idct 16x16 size
const VO_S16 kg_IDCT_coef_for_t16_asm[16] = {
  64, 83, 64, 36, 89, 75, 50, 18,
  90, 87, 80, 70, 57, 43, 25, 9
};

// the coefficient array in idst 4x4 size
const VO_S16 kg_IDST_coef_for_t4_asm[16] = {
  29, 55, 74, 84,
  74, 74, 0, -74,
  84, -29, -74, 55,
  55, -84, 74, -29
};

// the coefficient array in idct 32x32 size, which are for computing EEO[]
const VO_S16 kg_IDCT_coef_for_t32_asm_EEO[4 * 4] = {
  89, 75, 50, 18,
  75, -18, -89, -50,
  50, -89, 18, 75,
  18, -50, 75, -89
};

// the coefficient array in idct 32x32 size, which are for computing EO[]
const VO_S16 kg_IDCT_coef_for_t32_asm_EO[8 * 8] = {
  90, 87, 80, 70, 57, 43, 25, 9,
  87, 57, 9, -43, -80, -90, -70, -25,
  80, 9, -70, -87, -25, 57, 90, 43,
  70, -43, -87, 9, 90, 25, -80, -57,
  57, -80, -25, 90, -9, -87, 43, 70,
  43, -90, 57, 25, -87, 70, 9, -80,
  25, -70, 90, -80, 43, 9, -57, 87,
  9, -25, 43, -57, 70, -80, 87, -90
};

// the coefficient array in idct 32x32 size, which are for computing O[]
const VO_S16 kg_IDCT_coef_for_t32_asm_O[16 * 16] = {
  90, 90, 88, 85, 82, 78, 73, 67, 61, 54, 46, 38, 31, 22, 13, 4,
  90, 82, 67, 46, 22, -4, -31, -54, -73, -85, -90, -88, -78, -61, -38, -13,
  88, 67, 31, -13, -54, -82, -90, -78, -46, -4, 38, 73, 90, 85, 61, 22,
  85, 46, -13, -67, -90, -73, -22, 38, 82, 88, 54, -4, -61, -90, -78, -31,
  82, 22, -54, -90, -61, 13, 78, 85, 31, -46, -90, -67, 4, 73, 88, 38,
  78, -4, -82, -73, 13, 85, 67, -22, -88, -61, 31, 90, 54, -38, -90, -46,
  73, -31, -90, -22, 78, 67, -38, -90, -13, 82, 61, -46, -88, -4, 85, 54,
  67, -54, -78, 38, 85, -22, -90, 4, 90, 13, -88, -31, 82, 46, -73, -61,
  61, -73, -46, 82, 31, -88, -13, 90, -4, -90, 22, 85, -38, -78, 54, 67,
  54, -85, -4, 88, -46, -61, 82, 13, -90, 38, 67, -78, -22, 90, -31, -73,
  46, -90, 38, 54, -90, 31, 61, -88, 22, 67, -85, 13, 73, -82, 4, 78,
  38, -88, 73, -4, -67, 90, -46, -31, 85, -78, 13, 61, -90, 54, 22, -82,
  31, -78, 90, -61, 4, 54, -88, 82, -38, -22, 73, -90, 67, -13, -46, 85,
  22, -61, 85, -90, 73, -38, -4, 46, -78, 90, -82, 54, -13, -31, 67, -88,
  13, -38, 61, -78, 88, -90, 85, -73, 54, -31, 4, 22, -46, 67, -82, 90,
  4, -13, 22, -31, 38, -46, 54, -61, 67, -73, 78, -82, 85, -88, 90, -90
};

const VO_S16 kg_IDCT_coef_for_t32_4x4_asm_EEEO[18] = {                                                                         
  90, 87, 80, 70, 57, 43, 25, 9                                        
};
// the coefficient array in idct 32x32 size, which are for computing O[]
const VO_S16 kg_IDCT_coef_for_t32_4x4_asm_O[16 * 2] = 
{
  90, 90, 88, 85,   //10~13
  90, 82, 67, 46, 	//30~33
  31, 22, 13,  4,		//112~115
  -78,-61,-38,-13,	//312~315
  82, 78, 73, 67,		//14~17
  22, -4,-31,-54,		//34~37
  61, 54, 46, 38,		//18~11
  -73,-85,-90,-88,	//38~311
};
#else

// P_FUNCTION_POINT_FOR_IDCT IDCT_function_pointer_array[8] = {
//   IDCT4X4, IDCT8X8, 0, IDCT16X16, IDST4X4, 0, 0,0
// };
/*P_FUNCTION_POINT_FOR_IDCT IDCT_function_pointer_array[8] = {
  IDCT4X4, IDCT8X8, 0, IDCT16X16, IDST4X4, 0, 0,IDCT32X32
};*/
#endif
// the coefficient array in idct 4x4 size for c code.
const VO_S16 kg_IDCT_coef_for_t4[4 * 4] = {
  64, 64, 64, 64,
  83, 36, -36, -83,
  64, -64, -64, 64,
  36, -83, 83, -36
};

// the coefficient array in idct 8x8 size for c code.
const VO_S16 kg_IDCT_coef_for_t8[8 * 8] = {
  64, 64, 64, 64, 64, 64, 64, 64,
  89, 75, 50, 18, -18, -50, -75, -89,
  83, 36, -36, -83, -83, -36, 36, 83,
  75, -18, -89, -50, 50, 89, 18, -75,
  64, -64, -64, 64, 64, -64, -64, 64,
  50, -89, 18, 75, -75, -18, 89, -50,
  36, -83, 83, -36, -36, 83, -83, 36,
  18, -50, 75, -89, 89, -75, 50, -18
};


// the coefficient array in idct 16x16 size for c code.
const VO_S16 kg_IDCT_coef_for_t16[16 * 16] = {
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  90, 87, 80, 70, 57, 43, 25, 9, -9, -25, -43, -57, -70, -80, -87, -90,
  89, 75, 50, 18, -18, -50, -75, -89, -89, -75, -50, -18, 18, 50, 75, 89,
  87, 57, 9, -43, -80, -90, -70, -25, 25, 70, 90, 80, 43, -9, -57, -87,
  83, 36, -36, -83, -83, -36, 36, 83, 83, 36, -36, -83, -83, -36, 36, 83,
  80, 9, -70, -87, -25, 57, 90, 43, -43, -90, -57, 25, 87, 70, -9, -80,
  75, -18, -89, -50, 50, 89, 18, -75, -75, 18, 89, 50, -50, -89, -18, 75,
  70, -43, -87, 9, 90, 25, -80, -57, 57, 80, -25, -90, -9, 87, 43, -70,
  64, -64, -64, 64, 64, -64, -64, 64, 64, -64, -64, 64, 64, -64, -64, 64,
  57, -80, -25, 90, -9, -87, 43, 70, -70, -43, 87, 9, -90, 25, 80, -57,
  50, -89, 18, 75, -75, -18, 89, -50, -50, 89, -18, -75, 75, 18, -89, 50,
  43, -90, 57, 25, -87, 70, 9, -80, 80, -9, -70, 87, -25, -57, 90, -43,
  36, -83, 83, -36, -36, 83, -83, 36, 36, -83, 83, -36, -36, 83, -83, 36,
  25, -70, 90, -80, 43, 9, -57, 87, -87, 57, -9, -43, 80, -90, 70, -25,
  18, -50, 75, -89, 89, -75, 50, -18, -18, 50, -75, 89, -89, 75, -50, 18,
  9, -25, 43, -57, 70, -80, 87, -90, 90, -87, 80, -70, 57, -43, 25, -9
};

// the coefficient array in idct 32x32 size for c code.
const VO_S16 kg_IDCT_coef_for_t32[32 * 32] =
{
  64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
  90, 90, 88, 85, 82, 78, 73, 67, 61, 54, 46, 38, 31, 22, 13,  4, -4,-13,-22,-31,-38,-46,-54,-61,-67,-73,-78,-82,-85,-88,-90,-90,
  90, 87, 80, 70, 57, 43, 25,  9, -9,-25,-43,-57,-70,-80,-87,-90,-90,-87,-80,-70,-57,-43,-25, -9,  9, 25, 43, 57, 70, 80, 87, 90,
  90, 82, 67, 46, 22, -4,-31,-54,-73,-85,-90,-88,-78,-61,-38,-13, 13, 38, 61, 78, 88, 90, 85, 73, 54, 31,  4,-22,-46,-67,-82,-90,
  89, 75, 50, 18,-18,-50,-75,-89,-89,-75,-50,-18, 18, 50, 75, 89, 89, 75, 50, 18,-18,-50,-75,-89,-89,-75,-50,-18, 18, 50, 75, 89,
  88, 67, 31,-13,-54,-82,-90,-78,-46, -4, 38, 73, 90, 85, 61, 22,-22,-61,-85,-90,-73,-38,  4, 46, 78, 90, 82, 54, 13,-31,-67,-88,
  87, 57,  9,-43,-80,-90,-70,-25, 25, 70, 90, 80, 43, -9,-57,-87,-87,-57, -9, 43, 80, 90, 70, 25,-25,-70,-90,-80,-43,  9, 57, 87,
  85, 46,-13,-67,-90,-73,-22, 38, 82, 88, 54, -4,-61,-90,-78,-31, 31, 78, 90, 61,  4,-54,-88,-82,-38, 22, 73, 90, 67, 13,-46,-85,
  83, 36,-36,-83,-83,-36, 36, 83, 83, 36,-36,-83,-83,-36, 36, 83, 83, 36,-36,-83,-83,-36, 36, 83, 83, 36,-36,-83,-83,-36, 36, 83,
  82, 22,-54,-90,-61, 13, 78, 85, 31,-46,-90,-67,  4, 73, 88, 38,-38,-88,-73, -4, 67, 90, 46,-31,-85,-78,-13, 61, 90, 54,-22,-82,
  80,  9,-70,-87,-25, 57, 90, 43,-43,-90,-57, 25, 87, 70, -9,-80,-80, -9, 70, 87, 25,-57,-90,-43, 43, 90, 57,-25,-87,-70,  9, 80,
  78, -4,-82,-73, 13, 85, 67,-22,-88,-61, 31, 90, 54,-38,-90,-46, 46, 90, 38,-54,-90,-31, 61, 88, 22,-67,-85,-13, 73, 82,  4,-78,
  75,-18,-89,-50, 50, 89, 18,-75,-75, 18, 89, 50,-50,-89,-18, 75, 75,-18,-89,-50, 50, 89, 18,-75,-75, 18, 89, 50,-50,-89,-18, 75,
  73,-31,-90,-22, 78, 67,-38,-90,-13, 82, 61,-46,-88, -4, 85, 54,-54,-85,  4, 88, 46,-61,-82, 13, 90, 38,-67,-78, 22, 90, 31,-73,
  70,-43,-87,  9, 90, 25,-80,-57, 57, 80,-25,-90, -9, 87, 43,-70,-70, 43, 87, -9,-90,-25, 80, 57,-57,-80, 25, 90,  9,-87,-43, 70,
  67,-54,-78, 38, 85,-22,-90,  4, 90, 13,-88,-31, 82, 46,-73,-61, 61, 73,-46,-82, 31, 88,-13,-90, -4, 90, 22,-85,-38, 78, 54,-67,
  64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64,
  61,-73,-46, 82, 31,-88,-13, 90, -4,-90, 22, 85,-38,-78, 54, 67,-67,-54, 78, 38,-85,-22, 90,  4,-90, 13, 88,-31,-82, 46, 73,-61,
  57,-80,-25, 90, -9,-87, 43, 70,-70,-43, 87,  9,-90, 25, 80,-57,-57, 80, 25,-90,  9, 87,-43,-70, 70, 43,-87, -9, 90,-25,-80, 57,
  54,-85, -4, 88,-46,-61, 82, 13,-90, 38, 67,-78,-22, 90,-31,-73, 73, 31,-90, 22, 78,-67,-38, 90,-13,-82, 61, 46,-88,  4, 85,-54,
  50,-89, 18, 75,-75,-18, 89,-50,-50, 89,-18,-75, 75, 18,-89, 50, 50,-89, 18, 75,-75,-18, 89,-50,-50, 89,-18,-75, 75, 18,-89, 50,
  46,-90, 38, 54,-90, 31, 61,-88, 22, 67,-85, 13, 73,-82,  4, 78,-78, -4, 82,-73,-13, 85,-67,-22, 88,-61,-31, 90,-54,-38, 90,-46,
  43,-90, 57, 25,-87, 70,  9,-80, 80, -9,-70, 87,-25,-57, 90,-43,-43, 90,-57,-25, 87,-70, -9, 80,-80,  9, 70,-87, 25, 57,-90, 43,
  38,-88, 73, -4,-67, 90,-46,-31, 85,-78, 13, 61,-90, 54, 22,-82, 82,-22,-54, 90,-61,-13, 78,-85, 31, 46,-90, 67,  4,-73, 88,-38,
  36,-83, 83,-36,-36, 83,-83, 36, 36,-83, 83,-36,-36, 83,-83, 36, 36,-83, 83,-36,-36, 83,-83, 36, 36,-83, 83,-36,-36, 83,-83, 36,
  31,-78, 90,-61,  4, 54,-88, 82,-38,-22, 73,-90, 67,-13,-46, 85,-85, 46, 13,-67, 90,-73, 22, 38,-82, 88,-54, -4, 61,-90, 78,-31,
  25,-70, 90,-80, 43,  9,-57, 87,-87, 57, -9,-43, 80,-90, 70,-25,-25, 70,-90, 80,-43, -9, 57,-87, 87,-57,  9, 43,-80, 90,-70, 25,
  22,-61, 85,-90, 73,-38, -4, 46,-78, 90,-82, 54,-13,-31, 67,-88, 88,-67, 31, 13,-54, 82,-90, 78,-46,  4, 38,-73, 90,-85, 61,-22,
  18,-50, 75,-89, 89,-75, 50,-18,-18, 50,-75, 89,-89, 75,-50, 18, 18,-50, 75,-89, 89,-75, 50,-18,-18, 50,-75, 89,-89, 75,-50, 18,
  13,-38, 61,-78, 88,-90, 85,-73, 54,-31,  4, 22,-46, 67,-82, 90,-90, 82,-67, 46,-22, -4, 31,-54, 73,-85, 90,-88, 78,-61, 38,-13,
  9,-25, 43,-57, 70,-80, 87,-90, 90,-87, 80,-70, 57,-43, 25, -9, -9, 25,-43, 57,-70, 80,-87, 90,-90, 87,-80, 70,-57, 43,-25,  9,
  4,-13, 22,-31, 38,-46, 54,-61, 67,-73, 78,-82, 85,-88, 90,-90, 90,-90, 88,-85, 82,-78, 73,-67, 61,-54, 46,-38, 31,-22, 13, -4
};
// TODO(Joyce): need delete the global variable.



#if IDCT_ASM_ENABLED == 0
static VO_U32 g_bit_increment = 0;
extern VO_U32 g_IBDI_MAX;
/**
 * sin invtransform at 4x4
 * \param p_invtransform_buff   [IN]       dequant coef data which need be transformed.
 * \param p_prediction_buff     [IN]       other base information.
 * \param p_reconstruction_buff [OUT]      the reconstruction pixel data.
 * \param reconstruction_stride [IN]       stride of the reconstruction pixel buffer.
 * \retval VO_VOID.
 */
VO_VOID IDST4X4(const VO_S16 *p_invtransform_buff, 
             const VO_U8 *p_perdiction_buff,
             VO_U8 *p_reconstruction_buff,
             const VO_U32 reconstruction_stride,
             const VO_U32  prediction_stride,
             VO_S16 * tmp_block_array)
{
  VO_S32  shift_1st = SHIFT_INV_1ST;
#if FULL_NBIT
  VO_S32  shift_2nd = SHIFT_INV_2ND - ((VO_S16) g_bit_depth - 8);
#else
  VO_S32  shift_2nd = SHIFT_INV_2ND - g_bit_increment;
#endif

  VO_S32  i;
  VO_S32  c[4];
  VO_S16  tmp_block[4 * 4];
  VO_U8  *p_target_block = p_reconstruction_buff;
  VO_S32  rnd_factor = 1 << (shift_1st - 1);
  //VO_U32  prediction_stride = PRED_CACHE_STRIDE;

  for (i = 0; i < 4; i++) {
	  // Intermediate Variables
	  c[0] = p_invtransform_buff[i] + p_invtransform_buff[8 + i];
	  c[1] = p_invtransform_buff[8 + i] + p_invtransform_buff[12 + i];
	  c[2] = p_invtransform_buff[i] - p_invtransform_buff[12 + i];
	  c[3] = 74 * p_invtransform_buff[4 + i];

	  tmp_block[4 * i + 0] = (VO_S16) Clip3(-32768, 32767, (29 * c[0] + 
                            55 * c[1] + c[3] + rnd_factor) >> shift_1st);

	  tmp_block[4 * i + 1] = (VO_S16) Clip3(-32768, 32767, (55 * c[2] -
                            29 * c[1] + c[3] + rnd_factor) >> shift_1st);

	  tmp_block[4 * i + 2] = (VO_S16) Clip3(-32768, 32767, (74 * (p_invtransform_buff[i] -
                            p_invtransform_buff[8 + i] + p_invtransform_buff[12 + i]) + 
                            rnd_factor) >> shift_1st);

	  tmp_block[4 * i + 3] = (VO_S16) Clip3(-32768, 32767, (55 * c[0] + 29 * c[2] - c[3] +
                            rnd_factor) >> shift_1st);
  }

  rnd_factor = 1 << (shift_2nd - 1);
  for (i = 0; i < 4; i++) {
	  // Intermediate Variables
	  c[0] = tmp_block[i] + tmp_block[8 + i];
	  c[1] = tmp_block[8 + i] + tmp_block[12 + i];
	  c[2] = tmp_block[i] - tmp_block[12 + i];
	  c[3] = 74 * tmp_block[4 + i];

	  p_target_block[reconstruction_stride * i + 0] = (VO_U8) Clip3(0, 255, ((29 * c[0] + 
                                                    55 * c[1] + c[3] + rnd_factor) >> shift_2nd) +
                                                    p_perdiction_buff[prediction_stride * i + 0]);

	  p_target_block[reconstruction_stride * i + 1] = (VO_U8) Clip3(0, 255, ((55 * c[2] -
                                                    29 * c[1] + c[3] + rnd_factor) >> shift_2nd) +
                                                    p_perdiction_buff[prediction_stride * i + 1]);

	  p_target_block[reconstruction_stride * i + 2] = (VO_U8) Clip3(0, 255, ((74 * (tmp_block[i] -
                                                    tmp_block[8 + i] + tmp_block[12 + i]) + rnd_factor) >> 
                                                    shift_2nd) + p_perdiction_buff[prediction_stride * i + 2]);

	  p_target_block[reconstruction_stride * i + 3] = (VO_U8) Clip3(0, 255, ((55 * c[0] +
                                                    29 * c[2] - c[3] + rnd_factor) >> shift_2nd) +
                                                    p_perdiction_buff[prediction_stride * i + 3]);
    
  }

}



/**
 * cos invtransform at 4x4
 * \param p_invtransform_buff   [IN]       dequant coef data which need be transformed.
 * \param p_prediction_buff     [IN]       other base information.
 * \param p_reconstruction_buff [OUT]      the reconstruction pixel data.
 * \param reconstruction_stride [IN]       stride of the reconstruction pixel buffer.
 * \retval VO_VOID.
 */
VO_VOID IDCT4X4(const VO_S16 *p_invtransform_buff, 
             const VO_U8 *p_prediction_buff,
             VO_U8 *p_reconstruction_buff,
             const VO_U32 reconstruction_stride,
             const VO_U32  prediction_stride,
             VO_S16 * tmp_block_array)
{
  VO_S32  shift_1st = SHIFT_INV_1ST;
#if FULL_NBIT
  VO_S32  shift_2nd = SHIFT_INV_2ND - ((VO_S16) g_bit_depth - 8);
#else
  VO_S32  shift_2nd = SHIFT_INV_2ND - g_bit_increment;
#endif

  VO_S32  j;
  VO_S32  add = 1 << (shift_1st - 1);
  VO_S32  E[2];
  VO_S32  O[2];
  VO_S16  tmp_block[4 * 4];
  const VO_S16 *p_invtransform = p_invtransform_buff;
  const VO_U8 *p_prediction = p_prediction_buff;
  VO_U8  *p_target_block = p_reconstruction_buff;
  VO_S16 *p_tmp_block = tmp_block;
  //const VO_S16 *coef_t4 = kg_IDCT_coef_for_t4;
  //VO_U32  prediction_stride = PRED_CACHE_STRIDE;

  for (j = 0; j < 4; j++) {
	  // Utilizing symmetry properties to the maximum to minimize the number of multiplications
	  O[0] = 83 * p_invtransform[4] + 36 * p_invtransform[3 * 4];
	  O[1] = 36 * p_invtransform[4] - 83 * p_invtransform[3 * 4];
	  E[0] = 64 * (p_invtransform[0] + p_invtransform[2 * 4]);
	  E[1] = 64 * (p_invtransform[0] - p_invtransform[2 * 4]);

	  // Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector
	  p_tmp_block[0] = (VO_S16) Clip3(-32768, 32767, (E[0] + O[0] + add) >> shift_1st);
	  p_tmp_block[1] = (VO_S16) Clip3(-32768, 32767, (E[1] + O[1] + add) >> shift_1st);
	  p_tmp_block[2] = (VO_S16) Clip3(-32768, 32767, (E[1] - O[1] + add) >> shift_1st);
	  p_tmp_block[3] = (VO_S16) Clip3(-32768, 32767, (E[0] - O[0] + add) >> shift_1st);

	  p_invtransform++;
	  p_tmp_block += 4;
  }

  add = 1 << (shift_2nd - 1);
  p_tmp_block = tmp_block;
  for (j = 0; j < 4; j++) {
	  // Utilizing symmetry properties to the maximum to minimize the number of multiplications
	  O[0] = 83 * p_tmp_block[4] + 36 * p_tmp_block[3 * 4];
	  O[1] = 36 * p_tmp_block[4] - 83 * p_tmp_block[3 * 4];
	  E[0] = 64 * (p_tmp_block[0] + p_tmp_block[2 * 4]);
	  E[1] = 64 * (p_tmp_block[0] - p_tmp_block[2 * 4]);

	  // Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector
	  p_target_block[0] = (VO_U8) Clip3(0, 255, ((E[0] + O[0] + add) >> shift_2nd) + p_prediction[0]);
	  p_target_block[1] = (VO_U8) Clip3(0, 255, ((E[1] + O[1] + add) >> shift_2nd) + p_prediction[1]);
	  p_target_block[2] = (VO_U8) Clip3(0, 255, ((E[1] - O[1] + add) >> shift_2nd) + p_prediction[2]);
	  p_target_block[3] = (VO_U8) Clip3(0, 255, ((E[0] - O[0] + add) >> shift_2nd) + p_prediction[3]);

	  p_tmp_block++;
	  p_target_block += reconstruction_stride;
	  p_prediction += prediction_stride;
  }

}

/**
 * cos invtransform at 8x8
 * \param p_invtransform_buff   [IN]       dequant coef data which need be transformed.
 * \param p_prediction_buff     [IN]       other base information.
 * \param p_reconstruction_buff [OUT]      the reconstruction pixel data.
 * \param reconstruction_stride [IN]       stride of the reconstruction pixel buffer.
 * \retval VO_VOID.
 */
VO_VOID IDCT8X8(const VO_S16 *p_invtransform_buff,
             const VO_U8 *p_prediction_buff,
             VO_U8 *p_reconstruction_buff,
             const VO_U32 reconstruction_stride,
             const VO_U32  prediction_stride,
             VO_S16 * block_array)
{
  VO_S32  shift_1st = SHIFT_INV_1ST;
#if FULL_NBIT
  VO_S32  shift_2nd = SHIFT_INV_2ND - ((VO_S16) g_bit_depth - 8);
#else
  VO_S32  shift_2nd = SHIFT_INV_2ND - g_bit_increment;
#endif

  VO_S32  j, k;
  VO_S32  E[4];
  VO_S32  O[4];
  VO_S32  EE[2];
  VO_S32  EO[2];
  VO_S32  add = 1 << (shift_1st - 1);
  VO_S16  tmp_block_array[8 * 8];
  VO_S16 *p_tmp_block = tmp_block_array;
  const VO_S16 *p_invtransform = p_invtransform_buff;
  const VO_U8 *p_perdiction = p_prediction_buff;
  VO_U8  *p_target_block = p_reconstruction_buff;
  const VO_S16 *coef_t8 = kg_IDCT_coef_for_t8;
  //VO_U32  prediction_stride = PRED_CACHE_STRIDE;

  for (j = 0; j < 8; j++) {
	// Utilizing symmetry properties to the maximum to minimize the number of multiplications
	  for (k = 0; k < 4; k++) {
	    O[k] = coef_t8[1 * 8 + k] * p_invtransform[8] + coef_t8[3 * 8 + k] * p_invtransform[3 * 8] +
             coef_t8[5 * 8 + k] * p_invtransform[5 * 8] + coef_t8[7 * 8 + k] * p_invtransform[7 * 8];
	  }

	  EO[0] = coef_t8[2 * 8 + 0] * p_invtransform[2 * 8] + coef_t8[6 * 8 + 0] * p_invtransform[6 * 8];
	  EO[1] = coef_t8[2 * 8 + 1] * p_invtransform[2 * 8] + coef_t8[6 * 8 + 1] * p_invtransform[6 * 8];
	  EE[0] = coef_t8[0 * 8 + 0] * p_invtransform[0] + coef_t8[4 * 8 + 0] * p_invtransform[4 * 8];
	  EE[1] = coef_t8[0 * 8 + 1] * p_invtransform[0] + coef_t8[4 * 8 + 1] * p_invtransform[4 * 8];

	  // Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector
	  E[0] = EE[0] + EO[0];
	  E[3] = EE[0] - EO[0];
	  E[1] = EE[1] + EO[1];
	  E[2] = EE[1] - EO[1];
	  for (k = 0; k < 4; k++) {
	    p_tmp_block[k] = (VO_S16) Clip3(-32768, 32767, (E[k] + O[k] + add) >> shift_1st);
	    p_tmp_block[k + 4] = (VO_S16) Clip3(-32768, 32767, (E[3 - k] - O[3 - k] + add) >> shift_1st);
	  }
	  p_invtransform++;
	  p_tmp_block += 8;
  }

  add = 1 << (shift_2nd - 1);
  p_tmp_block = tmp_block_array;

  for (j = 0; j < 8; j++) {
	  // Utilizing symmetry properties to the maximum to minimize the number of multiplications 
	  for (k = 0; k < 4; k++) {
	    O[k] = coef_t8[1 * 8 + k] * p_tmp_block[8] + coef_t8[3 * 8 + k] * p_tmp_block[3 * 8] + 
             coef_t8[5 * 8 + k] * p_tmp_block[5 * 8] + coef_t8[7 * 8 + k] * p_tmp_block[7 * 8];
	  }

	  EO[0] = coef_t8[2 * 8 + 0] * p_tmp_block[2 * 8] + coef_t8[6 * 8 + 0] * p_tmp_block[6 * 8];
	  EO[1] = coef_t8[2 * 8 + 1] * p_tmp_block[2 * 8] + coef_t8[6 * 8 + 1] * p_tmp_block[6 * 8];
	  EE[0] = coef_t8[0 * 8 + 0] * p_tmp_block[0] + coef_t8[4 * 8 + 0] * p_tmp_block[4 * 8];
	  EE[1] = coef_t8[0 * 8 + 1] * p_tmp_block[0] + coef_t8[4 * 8 + 1] * p_tmp_block[4 * 8];

	  // Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector 
	  E[0] = EE[0] + EO[0];
	  E[3] = EE[0] - EO[0];
	  E[1] = EE[1] + EO[1];
	  E[2] = EE[1] - EO[1];
	  for (k = 0; k < 4; k++) {
	    p_target_block[k] = (VO_U8) Clip3(0, 255, ((E[k] + O[k] + add) >> shift_2nd) + p_perdiction[k]);

	    p_target_block[k + 4] = (VO_U8) Clip3(0, 255, ((E[3 - k] - O[3 - k] + add) >>
                              shift_2nd) + p_perdiction[k + 4]);

	  }
	  p_tmp_block++;
	  p_target_block += reconstruction_stride;
	  p_perdiction += prediction_stride;
  }

}

/**
 * cos invtransform at 16x16
 * \param p_invtransform_buff   [IN]       dequant coef data which need be transformed.
 * \param p_prediction_buff     [IN]       other base information.
 * \param p_reconstruction_buff [OUT]      the reconstruction pixel data.
 * \param reconstruction_stride [IN]       stride of the reconstruction pixel buffer.
 * \retval VO_VOID.
 */
VO_VOID IDCT16X16(const VO_S16 *p_invtransform_buff,
               const VO_U8 *p_prediction_buff,
               VO_U8 *p_reconstruction_buff,
               const VO_U32 reconstruction_stride,
               const VO_U32  prediction_stride,
               VO_S16 * block_array)
{
  VO_S32  shift_1st = SHIFT_INV_1ST;
#if FULL_NBIT
  VO_S32  shift_2nd = SHIFT_INV_2ND - ((VO_S16) g_bit_depth - 8);
#else
  VO_S32  shift_2nd = SHIFT_INV_2ND - g_bit_increment;
#endif

  VO_S32  j, k;
  VO_S32  E[8];
  VO_S32  O[8];
  VO_S32  EE[4];
  VO_S32  EO[4];
  VO_S32  EEE[2];
  VO_S32  EEO[2];
  VO_S32  add = 1 << (shift_1st - 1);

  VO_S16  tmp_block_array[16 * 16];
  VO_S16 *p_tmp_block = tmp_block_array;
  const VO_S16 *p_invtransform = p_invtransform_buff;
  const VO_U8 *p_prediction = p_prediction_buff;
  VO_U8  *p_reconstruction = p_reconstruction_buff;
  const VO_S16 *coef_t16 = kg_IDCT_coef_for_t16;
  //VO_U32  prediction_stride = PRED_CACHE_STRIDE;

  for (j = 0; j < 16; j++) {
	  // Utilizing symmetry properties to the maximum to minimize the number of multiplications
	  for (k = 0; k < 8; k++) {

	    O[k] =
		  coef_t16[1 * 16 + k] * p_invtransform[16] + coef_t16[3 * 16 + k] * p_invtransform[3 * 16] +
		  coef_t16[5 * 16 + k] * p_invtransform[5 * 16] + coef_t16[7 * 16 + k] * p_invtransform[7 * 16] +
		  coef_t16[9 * 16 + k] * p_invtransform[9 * 16] + coef_t16[11 * 16 + k] * p_invtransform[11 * 16] +
      coef_t16[13 * 16 + k] * p_invtransform[13 * 16] + coef_t16[15 * 16 + k] * p_invtransform[15 * 16];

	  }
	  for (k = 0; k < 4; k++) {
	    EO[k] = coef_t16[2 * 16 + k] * p_invtransform[2 * 16] + coef_t16[6 * 16 + k] * p_invtransform[6 * 16] +
              coef_t16[10 * 16 + k] * p_invtransform[10 * 16] + coef_t16[14 * 16 + k] * p_invtransform[14 * 16];
	  }
	  EEO[0] = coef_t16[4 * 16 + 0] * p_invtransform[4 * 16] + coef_t16[12 * 16 + 0] * p_invtransform[12 * 16];
	  EEE[0] = coef_t16[0 * 16 + 0] * p_invtransform[0] + coef_t16[8 * 16 + 0] * p_invtransform[8 * 16];
	  EEO[1] = coef_t16[4 * 16 + 1] * p_invtransform[4 * 16] + coef_t16[12 * 16 + 1] * p_invtransform[12 * 16];
	  EEE[1] = coef_t16[0 * 16 + 1] * p_invtransform[0] + coef_t16[8 * 16 + 1] * p_invtransform[8 * 16];

	  // Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector 
	  for (k = 0; k < 2; k++) {
	    EE[k] = EEE[k] + EEO[k];
	    EE[k + 2] = EEE[1 - k] - EEO[1 - k];
	  }
	  for (k = 0; k < 4; k++) {
	    E[k] = EE[k] + EO[k];
	    E[k + 4] = EE[3 - k] - EO[3 - k];
	  }
	  for (k = 0; k < 8; k++) {
	    p_tmp_block[k] = (VO_S16) Clip3(-32768, 32767, (E[k] + O[k] + add) >> shift_1st);
	    p_tmp_block[k + 8] = (VO_S16) Clip3(-32768, 32767, (E[7 - k] - O[7 - k] + add) >> shift_1st);
	  }
	  p_invtransform++;
	  p_tmp_block += 16;
  }

  p_tmp_block = tmp_block_array;
  add = 1 << (shift_2nd - 1);
  for (j = 0; j < 16; j++) {
	// Utilizing symmetry properties to the maximum to minimize the number of multiplications 
	for (k = 0; k < 8; k++) {
	  O[k] = coef_t16[1 * 16 + k] * p_tmp_block[16] + coef_t16[3 * 16 + k] * p_tmp_block[3 * 16] +
		       coef_t16[5 * 16 + k] * p_tmp_block[5 * 16] + coef_t16[7 * 16 + k] * p_tmp_block[7 * 16] +
		       coef_t16[9 * 16 + k] * p_tmp_block[9 * 16] + coef_t16[11 * 16 + k] * p_tmp_block[11 * 16] +
           coef_t16[13 * 16 + k] * p_tmp_block[13 * 16] + coef_t16[15 * 16 + k] * p_tmp_block[15 * 16];
	}
	for (k = 0; k < 4; k++) {
	  EO[k] = coef_t16[2 * 16 + k] * p_tmp_block[2 * 16] + coef_t16[6 * 16 + k] * p_tmp_block[6 * 16] + 
            coef_t16[10 * 16 + k] * p_tmp_block[10 * 16] + coef_t16[14 * 16 + k] * p_tmp_block[14 * 16];
	}
	EEO[0] = coef_t16[4 * 16 + 0] * p_tmp_block[4 * 16] + coef_t16[12 * 16 + 0] * p_tmp_block[12 * 16];
	EEE[0] = coef_t16[0 * 16 + 0] * p_tmp_block[0] + coef_t16[8 * 16 + 0] * p_tmp_block[8 * 16];
	EEO[1] = coef_t16[4 * 16 + 1] * p_tmp_block[4 * 16] + coef_t16[12 * 16 + 1] * p_tmp_block[12 * 16];
	EEE[1] = coef_t16[0 * 16 + 1] * p_tmp_block[0] + coef_t16[8 * 16 + 1] * p_tmp_block[8 * 16];

	// Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector 
	for (k = 0; k < 2; k++) {
	  EE[k] = EEE[k] + EEO[k];
	  EE[k + 2] = EEE[1 - k] - EEO[1 - k];
	}
	for (k = 0; k < 4; k++) {
	  E[k] = EE[k] + EO[k];
	  E[k + 4] = EE[3 - k] - EO[3 - k];
	}
	for (k = 0; k < 8; k++) {
	  p_reconstruction[k] = (VO_U8) Clip3(0, 255, ((E[k] + O[k] + add) >> shift_2nd) + p_prediction[k]);
	  p_reconstruction[k + 8] = (VO_U8) Clip3(0, 255, ((E[7 - k] - O[7 - k] + add) >> shift_2nd) + p_prediction[k + 8]);
	}
	p_tmp_block++;
	p_reconstruction += reconstruction_stride;
	p_prediction += prediction_stride;
  }

}


/**
 * cos invtransform at 32x32
 * \param p_invtransform_buff   [IN]       dequant coef data which need be transformed.
 * \param p_prediction_buff     [IN]       other base information.
 * \param p_reconstruction_buff [OUT]      the reconstruction pixel data.
 * \param reconstruction_stride [IN]       stride of the reconstruction pixel buffer.
 * \retval VO_VOID.
 */
VO_VOID IDCT32X32(const VO_S16 * p_invtransform_buff,
               const VO_U8 * p_prediction_buff,
               VO_U8 * p_reconstruction_buff,
               const VO_U32 reconstruction_stride,
               const VO_U32 prediction_stride,
               VO_S16 * block_array)
{
  VO_S32  shift_1st = SHIFT_INV_1ST;
#if FULL_NBIT
  VO_S32  shift_2nd = SHIFT_INV_2ND - ((VO_S16) g_bit_depth - 8);
#else
  VO_S32  shift_2nd = SHIFT_INV_2ND - g_bit_increment;
#endif

  VO_S32  j;
  VO_S32  k;
  VO_S32  add = 1 << (shift_1st - 1);
  VO_S32  E[16];
  VO_S32  O[16];
  VO_S32  EE[8];
  VO_S32  EO[8];
  VO_S32  EEE[4];
  VO_S32  EEO[4];
  VO_S32  EEEE[2];
  VO_S32  EEEO[2];

  VO_S16  tmp_block_array[32 * 32];
  VO_S16 *p_tmp_block = tmp_block_array;
  const VO_S16 *p_invtransform = p_invtransform_buff;
  const VO_U8 *p_prediction = p_prediction_buff;
  VO_U8  *p_reconstruction = p_reconstruction_buff;
  const VO_S16 *coef_t32 = kg_IDCT_coef_for_t32;
  //VO_U32  prediction_stride = PRED_CACHE_STRIDE;

  for (j = 0; j < 32; j++) {
	// Utilizing symmetry properties to the maximum to minimize the number of multiplications 
	for (k = 0; k < 16; k++) {
	  O[k] = coef_t32[1 * 32 + k] * p_invtransform[32] + coef_t32[3 * 32 + k] * p_invtransform[3 * 32] +
		       coef_t32[5 * 32 + k] * p_invtransform[5 * 32] + coef_t32[7 * 32 + k] * p_invtransform[7 * 32] +
		       coef_t32[9 * 32 + k] * p_invtransform[9 * 32] + coef_t32[11 * 32 + k] * p_invtransform[11 * 32] +
		       coef_t32[13 * 32 + k] * p_invtransform[13 * 32] + coef_t32[15 * 32 + k] * p_invtransform[15 * 32] +
		       coef_t32[17 * 32 + k] * p_invtransform[17 * 32] + coef_t32[19 * 32 + k] * p_invtransform[19 * 32] +
		       coef_t32[21 * 32 + k] * p_invtransform[21 * 32] + coef_t32[23 * 32 + k] * p_invtransform[23 * 32] +
           coef_t32[25 * 32 + k] * p_invtransform[25 * 32] + coef_t32[27 * 32 + k] * p_invtransform[27 * 32] +
           coef_t32[29 * 32 + k] * p_invtransform[29 * 32] + coef_t32[31 * 32 + k] * p_invtransform[31 * 32];
	}
	for (k = 0; k < 8; k++) {
	  EO[k] = coef_t32[2 * 32 + k] * p_invtransform[2 * 32] + coef_t32[6 * 32 + k] * p_invtransform[6 * 32] +
		        coef_t32[10 * 32 + k] * p_invtransform[10 * 32] + coef_t32[14 * 32 + k] * p_invtransform[14 * 32] +
            coef_t32[18 * 32 + k] * p_invtransform[18 * 32] + coef_t32[22 * 32 + k] * p_invtransform[22 * 32] +
            coef_t32[26 * 32 + k] * p_invtransform[26 * 32] + coef_t32[30 * 32 + k] * p_invtransform[30 * 32];
	}
	for (k = 0; k < 4; k++) {
	  EEO[k] = coef_t32[4 * 32 + k] * p_invtransform[4 * 32] + coef_t32[12 * 32 + k] * p_invtransform[12 * 32] +
             coef_t32[20 * 32 + k] * p_invtransform[20 * 32] + coef_t32[28 * 32 + k] * p_invtransform[28 * 32];
	}
	EEEO[0] = coef_t32[8 * 32 + 0] * p_invtransform[8 * 32] + coef_t32[24 * 32 + 0] * p_invtransform[24 * 32];
	EEEO[1] = coef_t32[8 * 32 + 1] * p_invtransform[8 * 32] + coef_t32[24 * 32 + 1] * p_invtransform[24 * 32];
	EEEE[0] = coef_t32[0 * 32 + 0] * p_invtransform[0] + coef_t32[16 * 32 + 0] * p_invtransform[16 * 32];
	EEEE[1] = coef_t32[0 * 32 + 1] * p_invtransform[0] + coef_t32[16 * 32 + 1] * p_invtransform[16 * 32];

	// Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector
	EEE[0] = EEEE[0] + EEEO[0];
	EEE[3] = EEEE[0] - EEEO[0];
	EEE[1] = EEEE[1] + EEEO[1];
	EEE[2] = EEEE[1] - EEEO[1];
	for (k = 0; k < 4; k++) {
	  EE[k] = EEE[k] + EEO[k];
	  EE[k + 4] = EEE[3 - k] - EEO[3 - k];
	}
	for (k = 0; k < 8; k++) {
	  E[k] = EE[k] + EO[k];
	  E[k + 8] = EE[7 - k] - EO[7 - k];
	}
	for (k = 0; k < 16; k++) {
	  p_tmp_block[k] = (VO_S16) Clip3(-32768, 32767, (E[k] + O[k] + add) >> shift_1st);
	  p_tmp_block[k + 16] = (VO_S16) Clip3(-32768, 32767, (E[15 - k] - O[15 - k] + add) >> shift_1st);
	}
	p_invtransform++;
	p_tmp_block += 32;
  }

  add = 1 << (shift_2nd - 1);
  p_tmp_block = tmp_block_array;

  for (j = 0; j < 32; j++) {
	  // Utilizing symmetry properties to the maximum to minimize the number of multiplications
	  for (k = 0; k < 16; k++) {
	    O[k] = coef_t32[1 * 32 + k] * p_tmp_block[32] + coef_t32[3 * 32 + k] * p_tmp_block[3 * 32] +
		         coef_t32[5 * 32 + k] * p_tmp_block[5 * 32] + coef_t32[7 * 32 + k] * p_tmp_block[7 * 32] +
             coef_t32[9 * 32 + k] * p_tmp_block[9 * 32] + coef_t32[11 * 32 + k] * p_tmp_block[11 * 32] +
             coef_t32[13 * 32 + k] * p_tmp_block[13 * 32] +	coef_t32[15 * 32 + k] * p_tmp_block[15 * 32] +
             coef_t32[17 * 32 + k] * p_tmp_block[17 * 32] + coef_t32[19 * 32 + k] * p_tmp_block[19 * 32] +
             coef_t32[21 * 32 + k] * p_tmp_block[21 * 32] + coef_t32[23 * 32 + k] * p_tmp_block[23 * 32] +
             coef_t32[25 * 32 + k] * p_tmp_block[25 * 32] + coef_t32[27 * 32 + k] * p_tmp_block[27 * 32] +
             coef_t32[29 * 32 + k] * p_tmp_block[29 * 32] + coef_t32[31 * 32 + k] * p_tmp_block[31 * 32];
	  }
	  for (k = 0; k < 8; k++) {
	    EO[k] =	coef_t32[2 * 32 + k] * p_tmp_block[2 * 32] + coef_t32[6 * 32 + k] * p_tmp_block[6 * 32] +
              coef_t32[10 * 32 + k] * p_tmp_block[10 * 32] + coef_t32[14 * 32 + k] * p_tmp_block[14 * 32] +
              coef_t32[18 * 32 + k] * p_tmp_block[18 * 32] + coef_t32[22 * 32 + k] * p_tmp_block[22 * 32] +
              coef_t32[26 * 32 + k] * p_tmp_block[26 * 32] + coef_t32[30 * 32 + k] * p_tmp_block[30 * 32];
	  }
	  for (k = 0; k < 4; k++) {
	    EEO[k] = coef_t32[4 * 32 + k] * p_tmp_block[4 * 32] + coef_t32[12 * 32 + k] * p_tmp_block[12 * 32] +
               coef_t32[20 * 32 + k] * p_tmp_block[20 * 32] + coef_t32[28 * 32 + k] * p_tmp_block[28 * 32];
	  }
	  EEEO[0] = coef_t32[8 * 32 + 0] * p_tmp_block[8 * 32] + coef_t32[24 * 32 + 0] * p_tmp_block[24 * 32];
	  EEEO[1] = coef_t32[8 * 32 + 1] * p_tmp_block[8 * 32] + coef_t32[24 * 32 + 1] * p_tmp_block[24 * 32];
	  EEEE[0] = coef_t32[0 * 32 + 0] * p_tmp_block[0] + coef_t32[16 * 32 + 0] * p_tmp_block[16 * 32];
	  EEEE[1] = coef_t32[0 * 32 + 1] * p_tmp_block[0] + coef_t32[16 * 32 + 1] * p_tmp_block[16 * 32];

	  // Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector
	  EEE[0] = EEEE[0] + EEEO[0];
	  EEE[3] = EEEE[0] - EEEO[0];
	  EEE[1] = EEEE[1] + EEEO[1];
	  EEE[2] = EEEE[1] - EEEO[1];
	  for (k = 0; k < 4; k++) {
	    EE[k] = EEE[k] + EEO[k];
	    EE[k + 4] = EEE[3 - k] - EEO[3 - k];
	  }
	  for (k = 0; k < 8; k++) {
	    E[k] = EE[k] + EO[k];
	    E[k + 8] = EE[7 - k] - EO[7 - k];
	  }
	  for (k = 0; k < 16; k++) {
	    p_reconstruction[k] = (VO_U8) Clip3(0, 255, ((E[k] + O[k] + add) >> shift_2nd) + p_prediction[k]);
	    p_reconstruction[k + 16] = (VO_U8) Clip3(0, 255, ((E[15 - k] - O[15 - k] + add) >> shift_2nd) + p_prediction[k + 16]);
	  }
	  p_tmp_block++;
	  p_reconstruction += reconstruction_stride;
	  p_prediction += prediction_stride;
  }

}


/**
 * sin invtransform at 4x4
 * \param p_invtransform_buff   [IN]       dequant coef data which need be transformed.
 * \param p_prediction_buff     [IN]       other base information.
 * \param p_reconstruction_buff [OUT]      the reconstruction pixel data.
 * \param reconstruction_stride [IN]       stride of the reconstruction pixel buffer.
 * \retval VO_VOID.
 */
VO_VOID IDST4X4_1X1(const VO_S16 *p_invtransform_buff,
             const VO_U8 *p_prediction_buff,
             VO_U8 *p_reconstruction_buff,
             const VO_U32 reconstruction_stride,
             const VO_U32 prediction_stride,
             VO_S16 * tmp_block_array)
{
  const VO_S16 invtransformValue = p_invtransform_buff[0];
  VO_S32  shift_1st = SHIFT_INV_1ST;
#if FULL_NBIT
  VO_S32  shift_2nd = SHIFT_INV_2ND - ((VO_S16) g_bit_depth - 8);
#else
  VO_S32  shift_2nd = SHIFT_INV_2ND - g_bit_increment;
#endif

  VO_S32  i;
  VO_S16  tmp_block[4];
  VO_U8  *p_target_block = p_reconstruction_buff;
  VO_S32  rnd_factor = 1 << (shift_1st - 1);
  //VO_U32  prediction_stride = PRED_CACHE_STRIDE;
 
	  // Intermediate Variables
	  tmp_block[0] = (VO_S16) Clip3(-32768, 32767, (29 * invtransformValue + rnd_factor) >> shift_1st);
	  tmp_block[1] = (VO_S16) Clip3(-32768, 32767, (55 * invtransformValue + rnd_factor) >> shift_1st);
	  tmp_block[2] = (VO_S16) Clip3(-32768, 32767, (74 * invtransformValue + rnd_factor) >> shift_1st);
	  tmp_block[3] = (VO_S16) Clip3(-32768, 32767, (84 * invtransformValue + rnd_factor) >> shift_1st);

  rnd_factor = 1 << (shift_2nd - 1);
  for (i = 0; i < 4; i++) {
	  // Intermediate Variables
	  p_target_block[reconstruction_stride * i + 0] = (VO_U8) Clip3(0, 255, ((29 * tmp_block[i] + rnd_factor) >> shift_2nd) +
                                                    p_prediction_buff[prediction_stride * i + 0]);
	  p_target_block[reconstruction_stride * i + 1] = (VO_U8) Clip3(0, 255, ((55 * tmp_block[i] + rnd_factor) >> shift_2nd) + 
                                                    p_prediction_buff[prediction_stride * i + 1]);
	  p_target_block[reconstruction_stride * i + 2] = (VO_U8) Clip3(0, 255, ((74 * tmp_block[i] + rnd_factor) >> 
                                                    shift_2nd) + p_prediction_buff[prediction_stride * i + 2]);
	  p_target_block[reconstruction_stride * i + 3] = (VO_U8) Clip3(0, 255, ((84 * tmp_block[i] + rnd_factor) >> shift_2nd) +
                                                    p_prediction_buff[prediction_stride * i + 3]);
  }

}

/**
 * cos invtransform at IDCT16X16 when the non zero is in the left-top 4X4 block
 * \param p_invtransform_buff   [IN]       dequant coef data which need be transformed.
 * \param p_prediction_buff     [IN]       other  base information.
 * \param p_reconstruction_buff [OUT]      the reconstruction pixel data.
 * \param reconstruction_stride [IN]       stride of the reconstruction pixel buffer.
 * \retval VO_VOID.
 */
VO_VOID IDCT16X16_4X4(const VO_S16 *p_invtransform_buff,
                   const VO_U8 *p_prediction_buff,
                   VO_U8 *p_reconstruction_buff,
                   const VO_U32 reconstruction_stride,
                   const VO_U32 prediction_stride,
                   VO_S16 * block_array)	
{
  VO_S32  shift_1st = SHIFT_INV_1ST;
#if FULL_NBIT
  VO_S32  shift_2nd = SHIFT_INV_2ND - ((VO_S16) g_bit_depth - 8);
#else
  VO_S32  shift_2nd = SHIFT_INV_2ND - g_bit_increment;
#endif

  VO_S32  j, k;
  VO_S32  E[8] = {0};
  VO_S32  O[8] = {0};
  VO_S32  EO[4] = {0};
  VO_S32  add = 1 << (shift_1st - 1);

  VO_S16  tmp_block_array[16 * 16] = {0}; // must be set to 0
  VO_S16 *p_tmp_block = tmp_block_array;
  const VO_S16 *p_invtransform = p_invtransform_buff;
  const VO_U8 *p_prediction = p_prediction_buff;
  VO_U8  *p_reconstruction = p_reconstruction_buff;
  const VO_S16 *coef_t16 = kg_IDCT_coef_for_t16;
  //VO_U32  prediction_stride = PRED_CACHE_STRIDE;

  VO_S32 midValue = 0;
  for (j = 0; j < 4; j++) {
	  // Utilizing symmetry properties to the maximum to minimize the number of multiplications
	  for (k = 0; k < 8; k++) {
	    O[k] =
		  coef_t16[1 * 16 + k] * p_invtransform[16] + coef_t16[3 * 16 + k] * p_invtransform[3 * 16];
	  }
	  for (k = 0; k < 4; k++) {
	    EO[k] = coef_t16[2 * 16 + k] * p_invtransform[2 * 16];
	  }
	  midValue = 64 * p_invtransform[0];
	  
	  for (k = 0; k < 4; k++) {
	    E[k] = midValue + EO[k];
	    E[k + 4] = midValue - EO[3 - k];
	  }
	  for (k = 0; k < 8; k++) {
	    p_tmp_block[k] = (VO_S16) Clip3(-32768, 32767, (E[k] + O[k] + add) >> shift_1st);
	    p_tmp_block[k + 8] = (VO_S16) Clip3(-32768, 32767, (E[7 - k] - O[7 - k] + add) >> shift_1st);
	  }
	  p_invtransform++;
	  p_tmp_block += 16;
  }

  p_tmp_block = tmp_block_array;
  add = 1 << (shift_2nd - 1);
  for (j = 0; j < 16; j++) {
	// Utilizing symmetry properties to the maximum to minimize the number of multiplications 
	for (k = 0; k < 8; k++) {
	  O[k] = coef_t16[1 * 16 + k] * p_tmp_block[16] + coef_t16[3 * 16 + k] * p_tmp_block[3 * 16];
	}
  
	for (k = 0; k < 4; k++) {
	  EO[k] = coef_t16[2 * 16 + k] * p_tmp_block[2 * 16];
	}
	midValue = 64 * p_tmp_block[0];

	for (k = 0; k < 4; k++) {
	  E[k] = midValue + EO[k];
	  E[k + 4] = midValue - EO[3 - k];
	}
	for (k = 0; k < 8; k++) {
	  p_reconstruction[k] = (VO_U8) Clip3(0, 255, ((E[k] + O[k] + add) >> shift_2nd) + p_prediction[k]);
	  p_reconstruction[k + 8] = (VO_U8) Clip3(0, 255, ((E[7 - k] - O[7 - k] + add) >> shift_2nd) + p_prediction[k + 8]);
	}
	p_tmp_block++;
	p_reconstruction += reconstruction_stride;
	p_prediction += prediction_stride;
  }

}

/**
 * cos invtransform at 32x32
 * \param p_invtransform_buff   [IN]       dequant coef data which need be transformed.
 * \param p_prediction_buff     [IN]       other base information.
 * \param p_reconstruction_buff [OUT]      the reconstruction pixel data.
 * \param reconstruction_stride [IN]       stride of the reconstruction pixel buffer.
 * \retval VO_VOID.
 */
VO_VOID IDCT32X32_4X4(const VO_S16 * p_invtransform_buff,
                       const VO_U8 * p_prediction_buff,
                       VO_U8 * p_reconstruction_buff,
                       const VO_U32 reconstruction_stride,
                       const VO_U32 prediction_stride,
                       VO_S16 * block_array)
{
  VO_S32  shift_1st = SHIFT_INV_1ST;
#if FULL_NBIT
  VO_S32  shift_2nd = SHIFT_INV_2ND - ((VO_S16) g_bit_depth - 8);
#else
  VO_S32  shift_2nd = SHIFT_INV_2ND - g_bit_increment;
#endif

  VO_S32  j;
  VO_S32  k;
  VO_S32  add = 1 << (shift_1st - 1);
  VO_S32  E[16];
  VO_S32  O[16];
  VO_S32  EO[8];


  VO_S16 tmp_block_array[32 * 32] = {0};
  VO_S16 *p_tmp_block = tmp_block_array;
  const VO_S16 *p_invtransform = p_invtransform_buff;
  const VO_U8 *p_prediction = p_prediction_buff;
  VO_U8  *p_reconstruction = p_reconstruction_buff;
  const VO_S16 *coef_t32 = kg_IDCT_coef_for_t32;
  //VO_U32  prediction_stride = PRED_CACHE_STRIDE;
  VO_S32 midValue = 0;

  for (j = 0; j < 4; j++) {
	// Utilizing symmetry properties to the maximum to minimize the number of multiplications 
	for (k = 0; k < 16; k++) {
	  O[k] = coef_t32[1 * 32 + k] * p_invtransform[32] + coef_t32[3 * 32 + k] * p_invtransform[3 * 32];
	}
	for (k = 0; k < 8; k++) {
	  EO[k] = coef_t32[2 * 32 + k] * p_invtransform[2 * 32];
	}

  midValue = 64 * p_invtransform[0];
	// Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector

	for (k = 0; k < 8; k++) {
	  E[k] = midValue+ EO[k];
	  E[k + 8] = midValue - EO[7 - k];
	}
	for (k = 0; k < 16; k++) {
	  p_tmp_block[k] = (VO_S16) Clip3(-32768, 32767, (E[k] + O[k] + add) >> shift_1st);
	  p_tmp_block[k + 16] = (VO_S16) Clip3(-32768, 32767, (E[15 - k] - O[15 - k] + add) >> shift_1st);
	}
	p_invtransform++;
	p_tmp_block += 32;
  }
  //[note: p_tmp_block_array in the row[4~31] need to be set to 0;

  add = 1 << (shift_2nd - 1);
  p_tmp_block = tmp_block_array;

  for (j = 0; j < 32; j++) {
	  // Utilizing symmetry properties to the maximum to minimize the number of multiplications
	  for (k = 0; k < 16; k++) {
	    O[k] = coef_t32[1 * 32 + k] * p_tmp_block[32] + coef_t32[3 * 32 + k] * p_tmp_block[3 * 32];
	  }
	  for (k = 0; k < 8; k++) {
	    EO[k] =	coef_t32[2 * 32 + k] * p_tmp_block[2 * 32];
	  }
	
    midValue = 64 * p_tmp_block[0];
    // Combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector

    for (k = 0; k < 8; k++) {
        E[k] = midValue+ EO[k];
        E[k + 8] = midValue - EO[7 - k];
    }
	  for (k = 0; k < 16; k++) {
	    p_reconstruction[k] = (VO_U8) Clip3(0, 255, ((E[k] + O[k] + add) >> shift_2nd) + p_prediction[k]);
	    p_reconstruction[k + 16] = (VO_U8) Clip3(0, 255, ((E[15 - k] - O[15 - k] + add) >> shift_2nd) + p_prediction[k + 16]);
	  }
	  p_tmp_block++;
	  p_reconstruction += reconstruction_stride;
	  p_prediction += prediction_stride;
  }

}

/**
 * cos invtransform at 16x16 when the non zero is in the left-top 1X1 block
 * \param p_invtransform_buff   [IN]       dequant coef data which need be transformed.
 * \param p_prediction_buff     [IN]       other base information.
 * \param p_reconstruction_buff [OUT]      the reconstruction pixel data.
 * \param reconstruction_stride [IN]       stride of the reconstruction pixel buffer.
 * \retval VO_VOID.
 */
VO_VOID IDCT_NZ_1X1(const VO_S16 *p_invtransform_buff,
                   const VO_U8 *p_prediction_buff,
                   VO_U8 *p_reconstruction_buff,
                   const VO_U32 reconstruction_stride,
                   const VO_U32 prediction_stride,
                   VO_S16 * typeU32_width)	
{
    const VO_S16 invtransformValue = p_invtransform_buff[0];
    const VO_U32 tuWidth = typeU32_width[0];
    VO_U32  j, k;
    VO_S32  add = 1 << 6;

    const VO_U8 *p_prediction = p_prediction_buff;
    VO_U8  *p_reconstruction = p_reconstruction_buff;

    //VO_U32  prediction_stride = PRED_CACHE_STRIDE;

    VO_S32 resiValue = (invtransformValue + 1 + add) >> 7;

    for (j = 0; j < tuWidth; j++) {
        for (k = 0; k < tuWidth; k++) {
            p_reconstruction[k] = (VO_U8) Clip3(0, 255, resiValue + p_prediction[k]); 
        }
        p_reconstruction += reconstruction_stride;
        p_prediction += prediction_stride;
    }
}


#endif
