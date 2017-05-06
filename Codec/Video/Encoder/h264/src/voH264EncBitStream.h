/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#ifndef _VOH264ENC_BITSTREAM_H_
#define _VOH264ENC_BITSTREAM_H_

void IniSPS(H264ENC *pEncGlobal, OUT_PARAM *InternalParam );
void WriteSPS(H264ENC *pEncGlobal, BS_TYPE *s);
void WritePPS(H264ENC *pEncGlobal, BS_TYPE *s );

VO_S32  IniLevel( H264ENC *pEncGlobal, H264LEVEL **l);

#endif//_VOH264ENC_BITSTREAM_H_
