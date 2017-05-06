/** 
 * \file dradec.h
 * \brief VisualOn DRA Decoder API
 * \version 0.1
 * \date 11/21/2008 created
 * \author VisualOn
 */


#ifndef __VO_DRA_DEC_H_
#define __VO_DRA_DEC_H_

#include "sdkbase.h"
#include "drabase.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)

#define VODRADECRETURNCODE VOCOMMONRETURNCODE
/**
 * Initialize a DRA decoder instance use default settings.
 * \param phCodec [out] Return the DRA Decoder handle.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_OUT_OF_MEMORY Out of memory.
 */
VODRADECRETURNCODE VOCODECAPI voDRADecInit(HVOCODEC *phCodec);


/**
 * Decode one frame data.
 * \param hCodec [in] DRA decoder instance handle, returned by voDRADecInit().
 * \param pInData [in/out] Input buffer pointer and length. 
 *  When the function return, pInData->length indicates the actual size consumed by decoder.
 * \param pOutData [in/out] Output buffer pointer and length.The buffer alloc and release by caller.
 *  When the function return VORC_OK, pOutData->length indicates the actual output size.
 *  If the function return VORC_COM_OUTPUT_BUFFER_SMALL, pOutData->length indicates the maximum output buffer size. 
 * \param pOutFormat [out] Output format info. The pointer can be NULL if you don't care the format info. 
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_STATUS The decoder is not ready for decode. Usually this indicates more parameters needed. See voDRASetParameter().
 * \retval VORC_COM_OUTPUT_BUFFER_SMALL The size of output buffer is too small.
 * \remarks The input and output bufferes are both allocated by the caller.
 *  To get the maximum output size, set pOutData->length to 0, then the function will return VORC_COM_OUTPUT_BUFFER_SMALL,
 *  and pOutData->length is the maximum output size.
 */
VODRADECRETURNCODE VOCODECAPI voDRADecProcess(HVOCODEC hCodec, VOCODECDATABUFFER *pInData, VOCODECDATABUFFER *pOutData, VOCODECAUDIOFORMAT *pOutFormat);


/**
 * Close the DRA decoder instance, release any resource it used.
 * \param hCodec [in] DRA decoder instance handle, returned by voDRADecInit().
 * \return The function should always return VORC_OK.
 */
VODRADECRETURNCODE VOCODECAPI voDRADecUninit(HVOCODEC hCodec);


/**
 * Set parameter of the decoder instance.
 * \param hCodec [in] DRA decoder instance handle, returned by voDRADecInit().
 * \param nID [in] Parameter ID, Supports: VOID_COM_HEAD_DATA, VOID_DRA_OBJECTTYPE, VOID_DRA_FRAMETYPE, VOID_DRA_CHANNELSPEC,VOID_DRA_SELECTCHS
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
VODRADECRETURNCODE VOCODECAPI voDRADecSetParameter(HVOCODEC hCodec, LONG nID, LONG lValue);


/**
 * Get parameter of the decoder instance.
 * \param hCodec [in] DRA decoder instance handle, returned by voDRADecInit().
 * \param nID [in] Parameter ID, Supports: VOID_COM_AUDIO_FORMAT, VOID_COM_AUDIO_CHANNELS, VOID_COM_AUDIO_SAMPLERATE, VOID_DRA_OBJECTTYPE, VOID_DRA_FRAMETYPE, VOID_DRA_CHANNELSPEC
 * \param plValue [out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
VODRADECRETURNCODE VOCODECAPI voDRADecGetParameter(HVOCODEC hCodec, LONG nID, LONG *plValue);





#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* __VO_DRA_DEC_H_ */



