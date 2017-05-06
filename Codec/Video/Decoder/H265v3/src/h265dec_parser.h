 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265dec_parser.h
    \brief    parser functions 
    \author   
	\change
*/


#ifndef _H265DEC_PARSER_H_
#define _H265DEC_PARSER_H_
#include "voType.h"
#include "h265_decoder.h"

extern VO_S32 ParseSPS( H265_DEC* const p_dec,
    H265_DEC_SPS *sps_array,
	H265_DEC_RPS *rps_array,
	BIT_STREAM *p_bs );

extern VO_S32 ParsePPS( H265_DEC* const p_dec,
	H265_DEC_SPS* sps_array ,
	H265_DEC_PPS* pps_array , 
	BIT_STREAM *p_bs );

extern VO_S32 ParseVPS(H265_DEC* const p_dec, H265_DEC_VPS *p_vps,
	BIT_STREAM *p_bs );

extern VO_S32 ParseSliceParamID(H265_DEC* p_dec, 
	H265_DEC_SLICE *p_slice, 
	BIT_STREAM *p_bs);

extern VO_S32 ParseSliceHeader(/* H265_DEC* p_dec,*/ 
	H265_DEC_SLICE *p_slice, 
	BIT_STREAM *p_bs,
	const H265_DEC_RPS* const rps_array,
	const VO_S32 pre_poc ) ;


#endif							/* _H265DEC_PARSER_H_ */
