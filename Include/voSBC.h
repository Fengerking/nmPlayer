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


#ifndef __voSBC_H__
#define __voSBC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <voAudio.h>

/* SBC Param ID */
#define VO_PID_SBC_Mdoule				0x42341000 
#define VO_PID_SBC_INPUTSIZE			(VO_PID_SBC_Mdoule | 0x0001)	/*!< SBC Parameter ID for getting input buffer size */
#define	VO_PID_SBC_MAXOUTSIZE			(VO_PID_SBC_Mdoule | 0x0002)	/*!< SBC Parameter ID for getting max output buffer size */

/* SBC decoder error ID */
#define VO_ERR_SBC_Mdoule				0x82230000


/**
 * Get SBC deoder API interface
 * \param pDecHandle [out] Return the SBC Decoder handle.
 * \retval VO_ERR_NONE Succeeded.
 */
VO_S32 VO_API voGetSBCDecAPI (VO_AUDIO_CODECAPI * pDecHandle);

/**
 * Get SBC encoder API interface
 * \param pEncHandle [out] Return the SBC Encoder handle.
 * \retval VO_ERR_NONE Succeeded.
 */
VO_S32 VO_API voGetSBCEncAPI (VO_AUDIO_CODECAPI * pEncHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voSBC_H__
