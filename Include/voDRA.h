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


#ifndef __voDRA_H__
#define __voDRA_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voAudio.h>

/* DRA Param ID */
#define VO_PID_DRA_Mdoule				0x422E1000


/* DRA decoder error ID */
#define VO_ERR_DRA_Mdoule				0x822E0000
#define VO_ERR_DRA_INVFRAME				VO_ERR_DRA_Mdoule | 0x0001

/**
 * Get Audio codec API interface
 * \param pDecHandle [out] Return the DRA Decoder handle.
 * \retval VO_ERR_NONE Succeeded.
 */
VO_S32 VO_API voGetDRADecAPI (VO_AUDIO_CODECAPI * pDecHandle);

/**
 * Get audio codec API interface
 * \param pEncHandle [out] Return the DRA Encoder handle.
 * \retval VO_ERR_NONE Succeeded.
 */
VO_S32 VO_API voGetDRAEncAPI (VO_AUDIO_CODECAPI * pEncHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voDRA_H__
