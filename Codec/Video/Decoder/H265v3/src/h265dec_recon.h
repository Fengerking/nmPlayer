#ifndef __H265DEC_RECON_H__
#define __H265DEC_RECON_H__

#include "h265_decoder.h"

//extern VO_VOID xReconInter(H265_DEC * p_dec, H265_DEC_SLICE *p_slice, VO_U32 uiAbsPartIdx, VO_U32 uiDepth );
//extern VO_S32 SetQPforQuant( VO_S32 qpy, TextType eTxtType, VO_S32 qpBdOffset, VO_S32 chromaQPOffset);
//extern VO_VOID xReconIntraQT(H265_DEC * p_dec, H265_DEC_SLICE *p_slice,  VO_S32 x_orig, VO_S32 y_orig, VO_S32 cb_size_pixel, VO_U32 uiAbsPartIdx, VO_U32 uiDepth );

#if ZS_ENABLED
extern void H265_IntraLumaRecQTNew(H265_DEC *p_dec, H265_DEC_SLICE *p_slice, VO_S32 x_orig, VO_S32 y_orig, VO_S32 cb_size_pixel,VO_S32 blkIdx);
extern void H265_IntraChromaRecQTNew(H265_DEC *p_dec, H265_DEC_SLICE *p_slice, VO_S32 x_orig, VO_S32 y_orig, VO_S32 cb_size_pixel,  VO_S32 blkIdx, VO_S32 uv);
#endif
//VO_VOID xPredInterUni (H265_DEC *p_dec, H265_DEC_SLICE* p_slice, VO_S32 x_orig, VO_S32 y_orig, 
//                       VO_S32 iWidth, VO_S32 iHeight, RefPicList eRefPicList, TComPic* rpcYuvPred, PicMvField* p_motion_field );
//VO_VOID xPredInterBi (H265_DEC *p_dec,  H265_DEC_SLICE* p_slice, VO_S32 x_orig, VO_S32 y_orig, 
 //                     VO_S32 iWidth, VO_S32 iHeight, TComPic* rpcYuvPred, PicMvField* p_motion_field);

//VO_S32 xCheckIdenticalMotion (  H265_DEC_SLICE *p_slice ,PicMvField *motion_field,VO_U32 dir);
//VO_VOID xInterWeightedPred_Bi (H265_DEC *p_dec,  H265_DEC_SLICE* p_slice, VO_S32 x_orig, VO_S32 y_orig, 
//                               VO_S32 iWidth, VO_S32 iHeight, TComPic* rpcYuvPred, PicMvField* p_motion_field);

//VO_VOID xInterWeightedPred_Uni (H265_DEC *p_dec, H265_DEC_SLICE* p_slice, VO_S32 x_orig, VO_S32 y_orig, 
 //                               VO_S32 iWidth, VO_S32 iHeight, RefPicList eRefPicList, TComPic* rpcYuvPred, PicMvField* p_motion_field );

VO_VOID H265_IntraPrediction( const H265_DEC_SLICE * const p_slice,
	VO_S32 xTbCmp, 
	VO_S32 yTbCmp, 
	VO_S32 log2TrafoSize, 
	VO_S32 predModeIntra,
	VO_S32 cIdx,
    VO_U32 available_flag);

VO_S32 H265_MotionCompensation( const H265_DEC_SLICE* const p_slice,
	const VO_U32 xPb, 
	const VO_U32 yPb, 
	const VO_U32 nPbW, 
	const VO_U32 nPbH,
	PicMvField* p_PuMvField ,
	TReconTask * const reconTask) ;

#endif

