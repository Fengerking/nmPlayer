#ifndef _MV_PREDICTION_H_
#define _MV_PREDICTION_H_

void GetMotionVectorPredictorIn(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,int n, int cache_width, short *pmv,short  ref_frame,int list);
void GetMotionVectorPredictor16x8(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,int n, int list, int ref, short *pmv);
void GetMotionVectorPredictor8x16(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,int n, int list, int ref, short *pmv);
void prepare_direct_params(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, MotionVector *pmvl0, MotionVector *pmvl1,VO_S8 *l0_rFrame, VO_S8 *l1_rFrame);

#endif
