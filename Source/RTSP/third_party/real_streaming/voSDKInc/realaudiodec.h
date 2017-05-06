/** 
 * \file RealAudiodec.h
 * \brief VisualOn RealAudio Decoder API
 * \version 0.1
 * \date 10/27/2006 created
 * \author VisualOn
 */


#ifndef __VO_RealAudio_DEC_H_
#define __VO_RealAudio_DEC_H_

#include "RealAudiobase.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif
#pragma pack(push, 4)

#define VORealAudioDECRETURNCODE VOCOMMONRETURNCODE
/**
 * Initialize a RealAudio decoder instance use default settings.
 * \param phCodec [out] Return the RealAudio Decoder handle.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_OUT_OF_MEMORY Out of memory.
 */
VORealAudioDECRETURNCODE VOCODECAPI voRealAudioDecInit(HVOCODEC *phCodec);


/**
 * Decode one frame data.
 * \param hCodec [in] RealAudio decoder instance handle, returned by voRealAudioDecInit().
 * \param pInData [in/out] Input buffer pointer and length. 
 *  When the function return, pInData->length indicates the actual size consumed by decoder.
 * \param pOutData [in/out] Output buffer pointer and length. The buffer alloc and release by decoder
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_STATUS The decoder is not ready for decode. Usually this indicates more parameters needed. See voRealAudioSetParameter().
 */
VORealAudioDECRETURNCODE VOCODECAPI voRealAudioDecProcess(HVOCODEC hCodec, VOCODECDATABUFFER *pInData, VOCODECDATABUFFER *pOutData, VOCODECAUDIOFORMAT *pOutFormat);


/**
 * Close the RealAudio decoder instance, release any resource it used.
 * \param hCodec [in] RealAudio decoder instance handle, returned by voRealAudioDecInit().
 * \return The function should always return VORC_OK.
 */
VORealAudioDECRETURNCODE VOCODECAPI voRealAudioDecUninit(HVOCODEC hCodec);


/**
 * Set parameter of the decoder instance.
 * \param hCodec [in] RealAudio decoder instance handle, returned by voRealAudioDecInit().
 * \param nID [in] Parameter ID, Supports VOID_COM_HEAD_DATA(VOID_RealAudio_INIT_PARAM),VOID_RealAudio_RESET
 * \VOID_RealAudio_GETFIRSTFRAME,VOID_RealAudio_DISABLEDEBLOCK
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
VORealAudioDECRETURNCODE VOCODECAPI voRealAudioDecSetParameter(HVOCODEC hCodec, LONG nID, LONG lValue);


/**
 * Get parameter of the decoder instance.
 * \param hCodec [in] RealAudio decoder instance handle, returned by voRealAudioDecInit().
 * \param nID [in] Parameter ID, supports VOID_COM_AUDIO_FORMAT,VOID_RealAudio_MAXOUTPUTSAMLES,VOID_COM_AUDIO_CHANNELS,VOID_COM_AUDIO_SAMPLERATE
 * \param plValue [out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VORC_OK Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
VORealAudioDECRETURNCODE VOCODECAPI voRealAudioDecGetParameter(HVOCODEC hCodec, LONG nID, LONG *plValue);


#pragma pack(pop)

#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif

#endif /* __VO_RealAudio_DEC_H_ */



