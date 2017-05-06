
/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __VP8_DEC_RECON_H
#define __VP8_DEC_RECON_H
#include "voVP8DecMBlock.h"

#ifdef VOARMV7
void vo_B_DC_PRED_neon(unsigned char *Above ,unsigned char* left,unsigned char *predictor);
void vo_B_TM_PRED_neon(unsigned char *Above ,unsigned char* left, unsigned char top_left ,unsigned char *predictor);
void vo_B_VE_PRED_neon(unsigned char *Above,unsigned char top_left,unsigned char *predictor);
void vo_B_HE_PRED_neon(unsigned char *Left,unsigned char top_left,unsigned char *predictor);
void vo_B_LD_PRED_neon(unsigned char *Above,unsigned char *predictor);
void vo_B_RD_PRED_neon(unsigned char *pp,unsigned char *predictor);
void vp8_build_intra_predictors_mby_neon   (MACROBLOCKD *x,unsigned char* y,unsigned long stride,MODE_INFO *pModeInfo);
void vp8_build_intra_predictors_mby_s_neon (MACROBLOCKD *x,unsigned char* y,unsigned long stride,MODE_INFO *pModeInfo);
#define vp8_build_intra_predictors_mby     vp8_build_intra_predictors_mby_neon
#define vp8_build_intra_predictors_mby_s  vp8_build_intra_predictors_mby_s_neon
#elif VOARMV6
 extern void vp8_build_intra_predictors_mby_c (MACROBLOCKD *x,unsigned char* y,unsigned long stride);
 #define vp8_build_intra_predictors_mby  vp8_build_intra_predictors_mby_c
 #define vp8_build_intra_predictors_mby_s  vp8_build_intra_predictors_mby_c
#else
extern void vp8_build_intra_predictors_mby_c (MACROBLOCKD *x,unsigned char* y,unsigned long stride,MODE_INFO *pModeInfo);
#define vp8_build_intra_predictors_mby     vp8_build_intra_predictors_mby_c
#define vp8_build_intra_predictors_mby_s  vp8_build_intra_predictors_mby_c
#endif

void vp8_setup_intra_recon(YV12_BUFFER_CONFIG *ybf);
void vp8_predict_intra4x4(BLOCKD *x,int b_mode,unsigned char *predictor);
void vp8_intra_prediction_down_copy(MACROBLOCKD *x);
void vp8_build_intra_predictors_mbuv(MACROBLOCKD *x,unsigned char* u,unsigned char* v,unsigned long stride,MODE_INFO *pModeInfo);
void vp8_build_inter_predictors_nosplitmv_mb(MACROBLOCKD *x,unsigned char*y,unsigned char*u,unsigned char*v,unsigned long ystride,unsigned long uvstride,MODE_INFO *pModeInfo );
void vp8_build_inter_predictors_splitmv_mb_s(MACROBLOCKD *x,MODE_INFO* pModeInfo);
void vp8_build_inter_predictors_splitmv_mb(MACROBLOCKD *x,MODE_INFO* pModeInfo);
void vp8_build_inter_predictors_mb(MACROBLOCKD *x,unsigned char*y,unsigned char*u,unsigned char*v,unsigned long ystride,unsigned long uvstride );
#endif