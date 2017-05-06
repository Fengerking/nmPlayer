/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2007		            *
*								 	                                    *
************************************************************************/

#ifndef __MPEG2_HR_H_
#define __MPEG2_HR_H_
#include "voMpegPort.h"
#include "voMpeg2Decoder.h"
#include "voMpeg2DecGlobal.h"
#include "voMpegReadbits.h"

extern VOMPEG2DECRETURNCODE extension_and_user_data(MpegDecode* dec);
extern VOMPEG2DECRETURNCODE sequence_header( MpegDecode* dec );
extern VOMPEG2DECRETURNCODE mpeg_decode_extension(MpegDecode* dec);
extern VOMPEG2DECRETURNCODE group_header( MpegDecode* dec );
#endif//__MPEG2_HR_H_