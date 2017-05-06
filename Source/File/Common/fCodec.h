	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		fCodec.h

	Contains:	fourcc and twocc function define header file

	Written by:	East Zhou

	Change History (most recent first):
	2009-03-25		East		Create file

*******************************************************************************/

#ifndef __fCodec_H__
#define __fCodec_H__

#include "voAudio.h"
#include "voVideo.h"


#ifdef _VONAMESPACE
	namespace _VONAMESPACE{
#else
	#ifdef __cplusplus
		extern "C" {
	#endif /* __cplusplus */
#endif // _VONAMESPACE


/**
 * FOURCC -> Video Codec Type
 * \param uFourcc [in] The Video Codec FOURCC
 * \return value Video Codec Type
 */
VO_VIDEO_CODINGTYPE	fCodecGetVideoCodec(VO_U32 uFourcc);

/**
 * TWOCC -> Audio Codec Type
 * \param uTwocc [in] The Audio Codec TWOCC
 * \return value Audio Codec Type
 */
VO_AUDIO_CODINGTYPE	fCodecGetAudioCodec(VO_U16 uTwocc);

/**
 * If FOURCC is belong DivX
 * \param uFourcc [in] The Video Codec FOURCC
 * \return value: VO_TRUE - DivX, VO_FALSE - Other
 */
VO_BOOL	fCodecIsDivX(VO_U32 uFourcc);


/**
* If FOURCC is belong XviD
* \param uFourcc [in] The Video Codec FOURCC
* \return value: VO_TRUE - XviD, VO_FALSE - Other
 */
VO_BOOL	fCodecIsXviD(VO_U32 uFourcc);


#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // _VONAMESPACE

#endif // __fCodec_H__
