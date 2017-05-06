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
#ifndef __VO_AUDIO_MCDEC__H
#define __VO_AUDIO_MCDEC__H

#include "voAudio.h"
#include "voAMediaCodecCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get audio decoder API interface
 * \param pDecHandle [IN/OUT] Return the audio Decoder API handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetAudioMCDecAPI (VO_AUDIO_CODECAPI * pDecHandle);


#ifdef __cplusplus
}
#endif	
#endif
