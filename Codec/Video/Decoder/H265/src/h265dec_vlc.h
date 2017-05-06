/******************************************************************************************
*                                                                                         *
*  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
*                                                                                         *
*******************************************************************************************/

/** \file     h265dec_vlc.h
\brief    VLC related stuff 
\author   
\change
*/

#ifndef __H265DEC_VLC_H_
#define __H265DEC_VLC_H_
#include "h265dec_porting.h"
#include "h265dec_bits.h"
#include "h265dec_debug.h"

extern VO_S32 ReadUV(BIT_STREAM *pBits, VO_S32 LenInBits);
extern VO_S32 ReadSV(BIT_STREAM *pBits, VO_S32 LenInBits);

extern VO_S32 ReadSEV(BIT_STREAM *pBits);
extern VO_S32 ReadUEV(BIT_STREAM *pBits);


#define READ_UV(p_bs,   len, name)      ReadUV(p_bs,  len);
#define READ_SV(p_bs,   len, name)      ReadSV(p_bs,  len); 
#define READ_UEV(p_bs,       name)      ReadUEV(p_bs)     
#define READ_SEV(p_bs,       name)      ReadSEV(p_bs);      
#define READ_FLAG(p_bs,      name)      ReadOneBit(p_bs); 
#define READ_FLAG1(p_bs)                ReadOneBit(p_bs)     

extern VO_S32 ceil_log2( VO_S32 x );

#endif//__H265DEC_VLC_H_

