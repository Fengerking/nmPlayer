/** 
 * \file h264dec.h
 * \brief VisualOn H264 Decoder API
 * \version 0.1
 * \date 10/27/2006 created
 * \author VisualOn
 */


#ifndef __VO_H264_DEC_H_
#define __VO_H264_DEC_H_

#include "h264base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)

#define VOH264DECRETURNCODE VOCOMMONRETURNCODE

VOH264DECRETURNCODE VOCODECAPI voH264DecInit(HVOCODEC *phCodec);


VOH264DECRETURNCODE VOCODECAPI voH264DecProcess(HVOCODEC hCodec, VOCODECDATABUFFER *pInData, VOCODECVIDEOBUFFER *pOutData, VOCODECVIDEOFORMAT *pOutFormat);


VOH264DECRETURNCODE VOCODECAPI voH264DecUninit(HVOCODEC hCodec);


VOH264DECRETURNCODE VOCODECAPI voH264DecSetParameter(HVOCODEC hCodec, signed long nID, signed long lValue);

VOH264DECRETURNCODE VOCODECAPI voH264DecGetParameter(HVOCODEC hCodec, signed long nID, signed long *plValue);


#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* __VO_H264_DEC_H_ */



