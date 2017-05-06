#ifndef _MC_PREDICTION_H_
#define _MC_PREDICTION_H_

#include "global.h"
#include "mbuffer.h"


extern void perform_mc           (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int pred_dir, int i, int j,int size_mode);
//add by Really Yang 20110328
extern void perform_mc_single(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int pred_dir, int i, int j,int size_mode);
extern void perform_mc_bi(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int i, int j,int size_mode);
//extern void perform_mc_bi_wp(Macroblock *currMB, ColorPlane pl, StorablePicture *dec_picture, int i, int j, int block_size_x, int block_size_y,int size_mode);
//end of add
//add by Really Yang 20110411
typedef void (*MC_PREDICTION_LUMA_FUNC)(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride);
extern  MC_PREDICTION_LUMA_FUNC GetLumaBlock[7][16];
extern  MC_PREDICTION_LUMA_FUNC AddLumaBlock[7][16];

typedef void (*MC_PREDICTION_OFFSET_LUMA_FUNC)(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
extern  MC_PREDICTION_OFFSET_LUMA_FUNC GetOffsetLumaBlock[7][16];

//end of add
//add by Really Yang 20110418
typedef void (*MC_PREDICTION_CHROMA_FUNC)(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern  MC_PREDICTION_CHROMA_FUNC GetChromaBlock[7][4];
extern  MC_PREDICTION_CHROMA_FUNC AddChromaBlock[7][4];

typedef void (*MC_PREDICTION_OFFSET_CHROMA_FUNC)(VO_U8 *src,VO_S32 src_stride,VO_U8 *dst,VO_S32 dst_stride, 
													 VO_S32 wp_offset,VO_S32 w00, VO_S32 w01, VO_S32 w10, VO_S32 w11);
extern  MC_PREDICTION_OFFSET_CHROMA_FUNC OffsetChromaBlock[7][4];

//end of add
//add by Really Yang 20110413
typedef void (*MC_WEIGHTED_FUNC)(VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_scale,VO_S32 wp_offset,VO_S32 weight_denom);
extern  MC_WEIGHTED_FUNC GetWeightedBlock[10];

typedef void (*MC_OFFSET_FUNC)(VO_U8 *dst,VO_S32 dst_stride,VO_S32 wp_offset);
extern  MC_OFFSET_FUNC GetOffsetBlock[10];


//end of add
//add by Really Yang 20110413
typedef void (*MC_BI_FUNC)(VO_U8 *dst,VO_S32 dst_stride,VO_U8 *src,VO_S32 src_stride);
extern  MC_BI_FUNC GetBIBlock[10];
//end of add
//add by Really Yang 20110413
typedef void (*MC_BIW_FUNC)(VO_U8 *dst,VO_S32 dst_stride,VO_U8 *src,VO_S32 src_stride,VO_S32 wp_scale_l0, VO_S32 wp_scale_l1, VO_S32 wp_offset, VO_S32 weight_denom);
extern  MC_BIW_FUNC GetBIWBlock[10];
//end of add

#endif

