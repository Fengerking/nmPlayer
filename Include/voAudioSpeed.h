/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __voAUDIOSPEED_H__
#define __voAUDIOSPEED_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voAudio.h>

/* AudioSpeed Param ID */
#define VO_PID_AUDIOSPEED_MODULE									 0x42221000

#define VO_PID_AUDIOSPEED_RATE		     							(VO_PID_AUDIOSPEED_MODULE | 0x0001)	/*!< AudioSpeed Rate parameter*/
#define VO_PID_AUDIOSPEED_HIGHQUALITY                          (VO_PID_AUDIOSPEED_MODULE | 0x0002)	/*!< AudioSpeed High Quality Enable */
#define VO_PID_AUDIOSPEED_FLUSH									    (VO_PID_AUDIOSPEED_MODULE | 0x0003) /*!< AudioSpeed Flush operation ID >*/
#define VO_PID_AUDIOSPEED_BITS                                       (VO_PID_AUDIOSPEED_MODULE | 0x0004) /*!< AudioSpeed Sample Bits >*/

/* AudioSpeed decoder error ID */
#define VO_ERR_AUDIOSPEED_MODULE								0x82220000
#define VO_ERR_AUDIOSPEED_INVHEADER						VO_ERR_AUDIOSPEED_MODULE | 0x0001
#define VO_ERR_AUDIOSPEED_INVFRAME							VO_ERR_AUDIOSPEED_MODULE | 0x0002


/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the AudioSpeed Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetAudioSpeedAPI (VO_AUDIO_CODECAPI * pDecHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voAUDIOSPEED_H__
