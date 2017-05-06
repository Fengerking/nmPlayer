/** 
 * \file RealVideodec.h
 * \brief VisualOn RealVideo Decoder API
 * \version 0.1
 * \date 10/27/2006 created
 * \author VisualOn
 */


#ifndef __VO_RealVideo_DEC_H_
#define __VO_RealVideo_DEC_H_

#include "RealVideobase.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif
#pragma pack(push, 4)

#define VORealVideoDECRETURNCODE VOCOMMONRETURNCODE
/**
 * Initialize a RealVideo decoder instance use default settings.
 * \param phCodec [out] Return the RealVideo Decoder handle.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_OUT_OF_MEMORY Out of memory.
 */
VORealVideoDECRETURNCODE VOCODECAPI voRealVideoDecInit(HVOCODEC *phCodec);


/**
 * Decode one frame data.
 * \param hCodec [in] RealVideo decoder instance handle, returned by voRealVideoDecInit().
 * \param pInData [in/out] Input buffer pointer and length. 
 *  When the function return, pInData->length indicates the actual size consumed by decoder.
 * \param pOutData [in/out] Output buffer pointer and length. The buffer alloc and release by decoder
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_STATUS The decoder is not ready for decode. Usually this indicates more parameters needed. See voRealVideoSetParameter().
 */
VORealVideoDECRETURNCODE VOCODECAPI voRealVideoDecProcess(HVOCODEC hCodec, VOCODECDATABUFFER *pInData, VOCODECVIDEOBUFFER *pOutData, VOCODECVIDEOFORMAT *pOutFormat);


/**
 * Close the RealVideo decoder instance, release any resource it used.
 * \param hCodec [in] RealVideo decoder instance handle, returned by voRealVideoDecInit().
 * \return The function should always return VORC_OK.
 */
VORealVideoDECRETURNCODE VOCODECAPI voRealVideoDecUninit(HVOCODEC hCodec);


/**
 * Set parameter of the decoder instance.
 * \param hCodec [in] RealVideo decoder instance handle, returned by voRealVideoDecInit().
 * \param nID [in] Parameter ID, Supports VOID_COM_HEAD_DATA(VOID_RealVideo_INIT_PARAM), VOID_RealVideo_OUPUTBUFFER, ,VOID_RealVideo_FLUSH
 * \VOID_RealVideo_GETFIRSTFRAME,VOID_RealVideo_DISABLEDEBLOCK
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
VORealVideoDECRETURNCODE VOCODECAPI voRealVideoDecSetParameter(HVOCODEC hCodec, LONG nID, LONG lValue);


/**
 * Get parameter of the decoder instance.
 * \param hCodec [in] RealVideo decoder instance handle, returned by voRealVideoDecInit().
 * \param nID [in] Parameter ID, supports VOID_COM_VIDEO_FORMAT, VOID_COM_VIDEO_WIDTH, VOID_COM_VIDEO_HEIGHT,VOID_RealVideo_MAXOUTPUTSIZE
 * \param plValue [out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
VORealVideoDECRETURNCODE VOCODECAPI voRealVideoDecGetParameter(HVOCODEC hCodec, LONG nID, LONG *plValue);


#pragma pack(pop)

#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif

#endif /* __VO_RealVideo_DEC_H_ */



