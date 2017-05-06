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


#if USE_FRAME_THREAD
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
#if CHROMA_QP_EXTENSION
static const VO_U8 g_aucChromaScale[58]=
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,
	17,18,19,20,21,22,23,24,25,26,27,28,29,29,30,31,32,
	33,33,34,34,35,35,36,36,37,37,38,39,40,41,42,43,44,
	45,46,47,48,49,50,51
};
#else
static const VO_U8 g_aucChromaScale[52]=
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,
	12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
	28,29,29,30,31,32,32,33,34,34,35,35,36,36,37,37,
	37,38,38,38,39,39,39,39
};
#endif
static VOINLINE VO_VOID set_qp(H265_DEC_SLICE *p_slice,VO_S32 qp)
{
	//VO_S32 iQPBDOffset = (p_slice->p_sps->bit_depth_chroma - 8) * 6; //0
	VO_S32 tmp_qp;
    VO_S32 *pDequant_qp_buff = p_slice->entries[p_slice->currEntryNum].dequant_qp;
	pDequant_qp_buff[0] = qp;
	tmp_qp = Clip3(0, 57, qp + p_slice->p_pps->pps_chroma_qp_offset[0] );
	pDequant_qp_buff[1] = g_aucChromaScale[ tmp_qp ] ;
	tmp_qp = Clip3(0, 57, qp + p_slice->p_pps->pps_chroma_qp_offset[1] );
	pDequant_qp_buff[2] = g_aucChromaScale[ tmp_qp ];
}
#endif//__H265DEC_SLICE_H__