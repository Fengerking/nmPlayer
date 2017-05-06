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



#ifndef __voMP3_H__
#define __voMP3_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voAudio.h>



/* MP3 Param ID */
#define VO_PID_MP3_Mdoule				0x42221000

#define VO_MP3_Mdoule_FAIL				      -1
#define VO_MP3_Mdoule_SUCCEED				    0x0
#define VO_MP3_Mdoule_INPUT_BUFFER_SMALL				0x1

/* MP3 decoder error ID */
#define VO_ERR_MP3_Mdoule				0x82220000
#define VO_ERR_MP3_INVHEADER			VO_ERR_MP3_Mdoule | 0x0001
#define VO_ERR_MP3_INVFRAME				VO_ERR_MP3_Mdoule | 0x0002


/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the MP3 Decoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetMP3DecAPI (VO_AUDIO_CODECAPI * pDecHandle);

/**
 * Get audio codec API interface
 * \param pEncHandle [out] Return the MP3 Encoder handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetMP3EncAPI (VO_AUDIO_CODECAPI * pEncHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voAAC_H__
