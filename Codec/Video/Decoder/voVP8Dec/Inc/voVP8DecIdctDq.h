
/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __VP8_DEC_IDCTDQ_H
#define __VP8_DEC_IDCTDQ_H

#include "voVP8DecBoolCoder.h"

#ifdef VOARMV7
//extern void vp8_short_idct4x4llm_neon(short *input, short *output, int pitch);
extern void vp8_dequant_dc_idct_add_neon(short *input, short *dq, unsigned char *pred, unsigned char *dest, int pitch, int stride, int Dc);
extern void vp8_dequant_idct_add_neon(short *input, short *dq, unsigned char *pred, unsigned char *dest, int pitch, int stride);
extern void vp8_dc_only_idct_add_neon(short input_dc, unsigned char *pred_ptr, unsigned char *dst_ptr, int pitch, int stride);
extern void vp8_short_inv_walsh4x4_neon(short *input, short *output);
extern void vp8_short_inv_walsh4x4_1_neon(short *input, short *output);

void vp8_dequantize_b(BLOCKD *d,short* dq);
void vp8_dequant_dc_idct_add_y_block_neon(MACROBLOCKD *xd,short *dq);
void vp8_dequant_idct_add_uv_block_neon(MACROBLOCKD *xd,short *dq);
void vp8_dequant_idct_add_y_block_neon(MACROBLOCKD *xd,short *dq);

#define vp8_dequant_dc_idct_add		vp8_dequant_dc_idct_add_neon
#define vp8_dequant_idct_add			vp8_dequant_idct_add_neon
#define vp8_dc_only_idct_add			vp8_dc_only_idct_add_neon

#define vp8_dc_idct_add_y_block		vp8_dequant_dc_idct_add_y_block_neon
#define vp8_idct_add_y_block			vp8_dequant_idct_add_y_block_neon
#define vp8_idct_add_uv_block			vp8_dequant_idct_add_uv_block_neon

#define vp8_short_inv_walsh4x4			vp8_short_inv_walsh4x4_neon
#define vp8_short_inv_walsh4x4_1		vp8_short_inv_walsh4x4_1_neon

#elif VOARMV6
extern void vp8_dequant_dc_idct_add_v6(short *input, short *dq, unsigned char *pred, unsigned char *dest, int pitch, int stride, int Dc);
extern void vp8_dequant_idct_add_v6(short *input, short *dq, unsigned char *pred, unsigned char *dest, int pitch, int stride);
extern void vp8_dc_only_idct_add_v6(short input_dc, unsigned char *pred_ptr, unsigned char *dst_ptr, int pitch, int stride);
extern void vp8_short_inv_walsh4x4_v6(short *input, short *output);
extern void vp8_short_inv_walsh4x4_1_v6(short *input, short *output);

void vp8_dequantize_b(BLOCKD *d,short* dq);
void vp8_dequant_dc_idct_add_y_block_v6(MACROBLOCKD *xd,short *dq);
void vp8_dequant_idct_add_uv_block_v6(MACROBLOCKD *xd,short *dq);
void vp8_dequant_idct_add_y_block_v6(MACROBLOCKD *xd,short *dq);

#define vp8_dequant_dc_idct_add		vp8_dequant_dc_idct_add_v6
#define vp8_dequant_idct_add			vp8_dequant_idct_add_v6
#define vp8_dc_only_idct_add			vp8_dc_only_idct_add_v6

//zou 1230 idct
#define vp8_dc_idct_add_y_block		vp8_dequant_dc_idct_add_y_block_v6
#define vp8_idct_add_y_block			vp8_dequant_idct_add_y_block_v6
#define vp8_idct_add_uv_block			vp8_dequant_idct_add_uv_block_v6

#define vp8_short_inv_walsh4x4			vp8_short_inv_walsh4x4_v6
#define vp8_short_inv_walsh4x4_1		vp8_short_inv_walsh4x4_1_v6
#else
void vp8_dequant_dc_idct_add_c(short *input, short *dq, unsigned char *pred, unsigned char *dest, int pitch, int stride, int Dc);
void vp8_dequant_idct_add_c(short *input, short *dq, unsigned char *pred, unsigned char *dest, int pitch, int stride);
void vp8_dc_only_idct_add_c(short input_dc, unsigned char *pred_ptr, unsigned char *dst_ptr, int pitch, int stride);
void vp8_short_inv_walsh4x4_c(short *input, short *output);
void vp8_short_inv_walsh4x4_1_c(short *input, short *output); 

void vp8_dequantize_b(BLOCKD *d,short* dq);
void vp8_dequant_dc_idct_add_y_block_c(MACROBLOCKD *xd,short *dq);
void vp8_dequant_idct_add_uv_block_c(MACROBLOCKD *xd,short *dq);
void vp8_dequant_idct_add_y_block_c(MACROBLOCKD *xd,short *dq);

#define vp8_dequant_dc_idct_add		vp8_dequant_dc_idct_add_c
#define vp8_dequant_idct_add			vp8_dequant_idct_add_c
#define vp8_dc_only_idct_add			vp8_dc_only_idct_add_c

#define vp8_dc_idct_add_y_block		vp8_dequant_dc_idct_add_y_block_c
#define vp8_idct_add_y_block			vp8_dequant_idct_add_y_block_c
#define vp8_idct_add_uv_block			vp8_dequant_idct_add_uv_block_c

#define vp8_short_inv_walsh4x4			vp8_short_inv_walsh4x4_c
#define vp8_short_inv_walsh4x4_1		vp8_short_inv_walsh4x4_1_c
#endif




#endif