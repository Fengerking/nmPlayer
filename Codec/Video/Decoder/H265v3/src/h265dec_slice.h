 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265dec_slice.h
    \brief    slice retated functions
    \author   Renjie Yu
	\change
*/


#ifndef __H265DEC_SLICE_H__
#define __H265DEC_SLICE_H__

#include "h265_decoder.h"
extern VOINLINE VO_VOID set_qp(H265_DEC_SLICE *p_slice,VO_S32 qp);

#if USE_3D_WAVE_THREAD
VO_S32 PrepareDecodeFrame(H265_DEC* const p_dec, 
	H265_DEC_SLICE * const p_slice, 
	VO_U8** pp_inbuf, 
	VO_S32 *buf_len);

VO_S32 DoDecodeFrame(const H265_DEC* const p_dec, 
	H265_DEC_SLICE * const p_slice, 
	VO_U8* p_inbuf, 
	VO_S32 buf_len);

#else

VO_S32 DecodeNalu(H265_DEC* p_dec, 
	VO_U8 *p_inbuf, 
	VO_S32 inbuf_len, 
	VO_S32 *used_len, 
	VO_BOOL *p_new_pic );

#endif
VO_S32 ReconTask(TReconTask* cur_rctask, VO_U32 nRow, VO_U32 nColumn);
#endif//__H265DEC_SLICE_H__