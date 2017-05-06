/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/


#ifndef _VOI_EQUALIZER_H
#define _VOI_EQUALIZER_H

#include "voAudio.h"

/*!
* The max bands is 10	 	
*/
#define MAXFILTER	10

/*!
* the sample rate of the PCM
*/
typedef enum
{
	sr_48000, /*!<  sample rate is 48k*/
	sr_44100, /*!<  sample rate is 44.1k*/
	sr_32000, /*!< sample rate is 32k*/
	sr_22050, /*!<  sample rate is 22.05k*/
	sr_16000, /*!< sample rate is 16k*/
	sr_11000, /*!<  sample rate is 11k*/
	sr_8000,  /*!<  sample rate is  8k*/
}EQ_SAMPLE_RATE;

/*!
* the normal equalizer mode
*/
typedef enum
{
	NONE	= -1,/*!< no mode */
	OTHERS	= 0, /*!< other mode,  equalizer band value selected by user, in [-20, 20] */
	NEW_AGE = 1, /*!< new age, COEF: -1, 6,	6,	-2, -3, -1, -2, -2, 2,	2 */
	ROCK,		 /*!< rock,    COEF: 8,	5,	-5, -8, -3, 4,	9,	11, 11, 11*/
	PIOANO,		 /*!< piano,   COEF: 5,	4,	0,	6,	6,	4,	6,	9,	4,	6 */
	POP,		 /*!< pop,     COEF: -2, 5,	7,	8,	6,	6,	2,	0, -2, -2 */
	RAP,		 /*!< rap,     COEF: -3, -1, 4,	4,	-2,	-2, 2,	2,	6,	9 */
	JAZZ,		 /*!< jazz,    COEF: 2,	2,	4,	6,	6,	6,	0,	5,	9,	9 */
}EQ_EQUAL_MODE;

/*!
* the setParameter ID
*/
typedef enum
{
	VO_EQ_ONOFF			= 0,  /*!<  set equalzier used or not, if set nonzero, used, else no used, default no used */
	VO_SAMPLERATE  		= 1,  /*!<  set input pcm channel samplerate, see enum EQ_SAMPLE_RATE, default sr_44100 */
	VO_CHANNELNUM		= 2,  /*!<  set input pcm channel number, default 2 channel */
	VO_EQMODE			= 10, /*!<  set equalzier mode, see enum EQ_EQUAL_MODE,  default NEW_AGE */
	VO_EA_EQUAT_COEF	= 11, /*!<  set equalzier coef, it should be pointer of ten integer array, the value is in [-20, 20]*/
}EQ_PARAMETER_ID;

#ifdef __cplusplus
extern "C" {
#endif
/*!
 * initialize equalzier
 * This function should be called first once the graphic equalizer is modified.
 * \param phEQ [in/out] EQ instance handle;
 * \retval 0 successful,others fail
*/
VO_S32	VO_API voEQInit(VO_HANDLE *phEQ);

/*!
 * notify the equalizer to process the input data 
 * \param phEQ [in/out] equalizer instance handle;
 * \param pInData [in/out] Input buffer pointer and length. 
 *  When the function return, pInData->length indicates the actual size consumed by eq.
 * \param pOutData [in/out] Output buffer pointer and length. The buffer alloc and release by caller
 *  When the function return 0, pOutData->length indicates the actual output size.
 * \retval 0 successful,others fail
*/
VO_S32	VO_API voEQProcess(VO_HANDLE hCodec, VO_CODECBUFFER *pInData, VO_CODECBUFFER *pOutData);
/*!
 * clean up equalizer;
 *\param phEQ [in/out] equalizer instance handle;
 * \retval 0 successful,others fail
*/
VO_S32 VO_API voEQUnInit(VO_HANDLE hCodec);

/**
 * Set parameter of the equalizer instance.
 * \param hCodec [in] equalizer  instance handle
 * \param uParamID [in] Parameter ID
 * \param pData [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VO_ERR_NONE Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
VO_S32 VO_API voEQSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);

/**
 * Get parameter of the equalizer instance..
 * \param hCodec [in] Mequalizer  instance handle
 * \param nID [in] Parameter ID
 * \param plValue [out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VO_ERR_NONE Succeeded.
 * \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
 */
VO_S32 VO_API voEQGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);

/**
* Get the 10 frequency bands.
* \param wavFrameData [in].
* \param wavFrameSizeInBytes [in] the size of wav data in bytes. 
* \param channels	[in].channel number,the max value is 2.
* \param sampleRate [in].
* \param bands		[out].the value is [0..100]
* \param bandlength	[int].the value is the number of bands, in [10, 256]
* \retval VO_ERR_NONE Succeeded.
* \retval NON_ZERO Fail.
*/
VO_S32	VO_API voGetWavFreqBand(short* wavFrameData,
								int	wavFrameSizeInBytes,
								int	channels,
								int	sampleRate,
								int	*bands,
								int bandlength);
#ifdef __cplusplus
}
#endif
#endif//_VOI_EQUALIZER_H
