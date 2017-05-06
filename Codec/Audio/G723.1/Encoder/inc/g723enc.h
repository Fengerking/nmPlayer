/** 
 * \file g723enc.h
 * \brief VisualOn G723 Encoder API
 * \version 0.1
 * \date 25/11/2008 created
 * \author VisualOn
 */


#ifndef __VO_G723_ENC_H_
#define __VO_G723_ENC_H_

#include "sdkbase.h"
#include "g723base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)

#define VOG723ENCRETURNCODE VOCOMMONRETURNCODE
/**
 * Initialize a G723 encoder instance use default settings.
 * \param phCodec [out] Return the G723 Decoder handle.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_OUT_OF_MEMORY Out of memory.
 */
VOG723ENCRETURNCODE VOCODECAPI voG723EncInit(HVOCODEC *phCodec);


/**
 * Decode one frame data.
 * \param hCodec [in] G723 encoder instance handle, returned by voG723EncInit().
 * \param pInData [in/out] Input buffer pointer and length. 
 *  When the function return, pInData->length indicates the actual size consumed by encoder.
 * \param pOutData [out] Output buffer pointer and length. The buffer is allocated and released by user
 *  When the function return VORC_OK, pOutData->length indicates the actual output size.
 * \param pOutFormat [out] Output format info. The pointer can be NULL if you don't care the format info. 
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_STATUS The encoder is not ready for decode. Usually this indicates more parameters needed. See voG723SetParameter().
 * \retval VORC_COM_OUTPUT_BUFFER_SMALL The size of output buffer is too small.
 * \remarks The input and output buffers are both allocated by the caller.
 *  To get the maximum output size, set pOutData->length to 0, then the function will return VORC_COM_OUTPUT_BUFFER_SMALL,
 *  and pOutData->length is the maximum output size.
 */
VOG723ENCRETURNCODE VOCODECAPI voG723EncProcess(HVOCODEC hCodec, VOCODECDATABUFFER *pInData, VOCODECDATABUFFER *pOutData, VOCODECAUDIOFORMAT *pOutFormat);


/**
 * Close the G723 encoder instance, release any resource it used.
 * \param hCodec [in] G723 encoder instance handle, returned by voG723EncInit().
 * \return The function should always return VORC_OK.
 */
VOG723ENCRETURNCODE VOCODECAPI voG723EncUninit(HVOCODEC hCodec);


/**
 * Set parameter of the encoder instance.
 * \param hCodec [in] G723 encoder instance handle, returned by voG723EncInit().
 * \param nID [in] Parameter ID, Supports: VOID_G723_DATATYPE
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_INVALID_ARG No such parameter ID supported.
 */
VOG723ENCRETURNCODE VOCODECAPI voG723EncSetParameter(HVOCODEC hCodec, LONG nID, LONG lValue);


/**
 * Get parameter of the encoder instance.
 * \param hCodec [in] G723 encoder instance handle, returned by voG723EncInit().
 * \param nID [in] Parameter ID, Supports: VOID_COM_AUDIO_FORMAT, VOID_COM_AUDIO_CHANNELS, VOID_COM_AUDIO_SAMPLERATE.
 * \param plValue [out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_INVALID_ARG No such parameter ID supported.
 */
VOG723ENCRETURNCODE VOCODECAPI voG723EncGetParameter(HVOCODEC hCodec, LONG nID, LONG *plValue);


#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __VO_G723_ENC_H_ */


