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

#ifndef __voffmpegWrap_H__
#define __voffmpegWrap_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voVideo.h>
#include <voAudio.h>


/**
 * Get video decoder API interface
 * \param pDecHandle [IN/OUT] Return the video Decoder API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_OK Succeeded.
 */
//EXPORT VO_S32 VO_API voGetffmpegVideoDecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);
VO_S32 VO_API voGetH264DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);

VO_S32 VO_API voGetMPEG4DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag);

/**
 * Get A?udio decoder API interface
 * \param pDecHandle [IN/OUT] Return the audio Decoder API handle.
 * \param uFlag,reserved
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetffmpegAudioDecAPI (VO_AUDIO_CODECAPI * pDecHandle, VO_U32 uFlag);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voffmpegWrap_H__
