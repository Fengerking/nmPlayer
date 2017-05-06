

#ifndef _MB_PREDICTION_H_
#define _MB_PREDICTION_H_

extern int mb_pred_intra4x4      (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture);
extern int mb_pred_intra16x16    (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture);
extern int mb_pred_intra8x8(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture);
extern int mb_pred_p_inter8x8    (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture);
extern int mb_pred_b_d4x4spatial (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
extern int mb_pred_b_d8x8spatial (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
extern int mb_pred_b_d4x4temporal(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
extern int mb_pred_b_d8x8temporal(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
extern VO_S32 mb_pred_b_temporal(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type);
extern VO_S32 mb_pred_b_temporal_interlace(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type);
extern VO_S32 mb_pred_b_spatial(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type);
extern VO_S32 mb_pred_b_spatial_interlace(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type);
extern int mb_pred_b_inter8x8    (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture);
extern int mb_pred_ipcm          (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);



#endif
